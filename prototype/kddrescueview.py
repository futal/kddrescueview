#!/usr/bin/env python3

# Prototype to visualise GNU ddrescue mapfiles using Python and GLSL
# 
# Copyright 2020  Adrien Cordonnier <adrien.cordonnier@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import re
import sys
import logging
import numpy as np
from PyQt5 import QtWidgets, QtCore, QtOpenGL
import moderngl
from collections import namedtuple


class Scene:
    def __init__(self, ctx, reserve='16MB'):
        self.ctx = ctx
        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 vertices;

                void main() {
                    gl_Position = vec4(vertices, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330

                uniform int tex_size;
                uniform ivec2 resolution;
                uniform int levels;
                uniform int square_size = 8;
                uniform usampler3D tex;

                out vec4 gl_FragColor;

                void main() {
                    if(gl_FragCoord.x > resolution.x - mod(resolution.x - 1.0, square_size)) {
                        // right margin as there is not enough space for full squares
                        gl_FragColor = vec4(1.);
                        return;
                    }
                    
                    if(gl_FragCoord.y > resolution.y - mod(resolution.y - 1.0, square_size)) {
                        // top margin as there is not enough space for full squares
                        gl_FragColor = vec4(1.);
                        return;
                    }

                    if(mod(int(gl_FragCoord.x),square_size) == 0) {
                        // vertical grid bars
                        gl_FragColor = vec4(1.);
                        return;
                    }
                    
                    if(mod(int(gl_FragCoord.y),square_size) == 0) {
                        // horizontal grid bars
                        gl_FragColor = vec4(1.);
                        return;
                    }

                    ivec2 grid_coord = ivec2(gl_FragCoord.xy) / ivec2(square_size);
                    ivec2 grid_resolution = resolution / ivec2(square_size);
                    int square = grid_coord.y * grid_resolution.x + grid_coord.x;
                    int squares = grid_resolution.x * grid_resolution.y;
                    
                    ivec3 icoord;
                    uvec3 ucoord;
                
                    icoord = ivec3(mod(square, tex_size), 0, 0);
                    for(int i = 0; i < levels; ++i) {
                        ucoord = texelFetch(tex, icoord, 0).rgb;
                        icoord = ivec3(ucoord);
                    }
                    vec3 rgb = vec3(ucoord) / vec3(tex_size);
                    gl_FragColor = vec4(rgb, 1.0);
                }
            ''',
        )

        self.vbo = ctx.buffer(reserve='4MB', dynamic=True)
        self.vao = ctx.simple_vertex_array(self.prog, self.vbo, 'vertices')

        tex_size = 256
        self.prog['tex_size'] = tex_size
        tex_data = np.random.uniform(low=0, high=tex_size-1, size=(tex_size,)*3+(4,)).astype('u1')
        self.tex = ctx.texture3d(size=(tex_size,)*3, components=4, data=tex_data, alignment=1, dtype='u1')
        self.tex.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.tex.use()
        self.prog['resolution'] = (512, 512)
        self.prog['levels'] = 3
        self.prog['square_size'] = 8

    def clear(self, color=(0, 0, 0, 0)):
        self.ctx.clear(*color)

    def plot(self, points):
        data = points.astype('f4').tobytes()
        self.vbo.orphan()
        self.vbo.write(data)
        self.vao.render(moderngl.TRIANGLES, vertices=len(data))


vertices = np.array(
    # upper left triangle  bottom right triangle
    [ [-1.0, +1.0, -1.0,   +1.0, -1.0, +1.0],  # x
      [+1.0, +1.0, -1.0,   +1.0, -1.0, -1.0],  # y
    ]).T  # transpose for OpenGL


class Widget(QtOpenGL.QGLWidget):
    def __init__(self):
        fmt = QtOpenGL.QGLFormat()
        fmt.setVersion(3, 3)
        fmt.setProfile(QtOpenGL.QGLFormat.CoreProfile)
        fmt.setSampleBuffers(True)
        self.timer = QtCore.QElapsedTimer()        
        super(Widget, self).__init__(fmt, None)
        self.scene = None

    def paintGL(self):
        self.ctx = moderngl.create_context()
        self.screen = self.ctx.detect_framebuffer()
        self.init()
        self.render()
        self.paintGL = self.render

    def init(self):
        self.resize(512, 512)
        self.ctx.viewport = (0, 0, 512, 512)
        self.scene = Scene(self.ctx)

    def render(self):
        self.screen.use()
        self.scene.clear()
        self.scene.plot(vertices)



rescue_statuses = {
    '?': 'copying non-tried blocks',
    '*': 'trimming non-trimmed blocks',
    '/': 'scraping non-scraped blocks',
    '-': 'retrying bad sectors',
    'F': 'filling specified blocks',
    'G': 'generating approximate mapfile',
    '+': 'finished',
}

block_statuses = {
    '?': 'non-tried block',
    '*': 'failed block non-trimmed',
    '/': 'failed block non-scraped',
    '-': 'failed block bad-sector(s)',
    '+': 'finished block',
}

Block = namedtuple('Block', 'start size status')
Block.end = property(lambda self: self.start + self.size)

class Rescue:
    def __init__(self, mapfile):
        self.commandline = None
        self.sector_size = None
        self.current_position = None
        self.current_status = None
        self.current_pass = None
        self.blocks = list()
        self.parse(mapfile)
        self.sector_size = self.sector_size or 512

    def parse(self, mapfile):
        with open(mapfile) as file:
            lines = file.readlines()

        for n, line in enumerate(lines):
            line = line.strip()
            
            if not line:
                logging.debug(f'line {n}: empty line')
                continue

            if self.commandline and line.startswith('# Command line:'):
                logging.error(f'line {n}: more than one command line')
                continue

            if line.startswith('# Command line:'):
                logging.debug(f'line {n}: command line')
                self.commandline = line
                match = re.search(r'(-b|--block-size=)\s*(?P<blocksize>[0-9]+)', line)
                self.sector_size = int(match.group('blocksize')) if match else 512  # TODO: if conversion fails
                logging.info(f'sector_size = {self.sector_size}')
                continue

            if line.startswith('#'):
                logging.debug(f'line {n}: comment line')
                continue

            parts = line.split()

            if len(parts) == 2 or len(parts) > 2 and parts[2].strip().startswith('#'):
                try:
                    position = int(parts[0], 0)
                    status = parts[0].strip()
                except:
                    position = None
                if position and status in statuses: 
                    self.position = position
                    self.status = status
                    logging.debug(f'line {n}: old status line')
                    continue
                else:
                    logging.warning(f'line {n}: incorrect old status line')
                    continue

            if len(parts) == 3 or len(parts) > 3 and parts[3].strip().startswith('#'):
                try:  # is it a status line (new format)
                    start = int(parts[0], 0)
                    size = int(parts[1], 0)
                    status = parts[2].strip()
                except ValueError:
                    logging.debug(f'line {n}: not a block line')
                else:
                    if status not in block_statuses or start < 0 or size <= 0:
                        logging.error(f'line {n}: incorrect block line ({line})')
                        continue
                    logging.debug(f'line {n}: block line')
                    self.blocks.append(Block(start, size, status))
                    continue

                try:  # is it a block line
                    position = int(parts[0], 0)
                    status = parts[1].strip()
                    passnumber = int(parts[2], 0)
                except ValueError:
                    logging.debug(f'line {n}: not a new rescue status line')
                else:
                    if status not in rescue_statuses or position < 0 or passnumber < 1:
                        logging.error(f'line {n}: incorrect new status line')
                        continue
                    logging.debug(f'line {n}: status line (new format)')
                    self.current_position = position
                    self.current_status = status
                    self.current_pass = passnumber
                    continue

            logging.warning(f'line {n}: invalid line not processed ({line})')

    start = property(lambda self: self.blocks[0].start)
    end = property(lambda self: self.blocks[-1].start + self.blocks[-1].size)
    size = property(lambda self: self.end - self.start)


def merge(list1, list2):
    # check that both lists have the same start and end
    if list1[0].start != list2[0].start:
        logging.error(f'the lists do not have the same start: {list1[0].start} and {list2[0].start}')
        return
    if list1[-1].end != list2[-1].end:
        logging.error(f'the lists do not have the same end: {list1[-1].end} and {list2[-1].end}')
        return

    iter1 = iter(list1)
    iter2 = iter(list2)
    result = list()
    item1 = next(iter1, None)
    item2 = next(iter2, None)
    while item1 and item2:
        part = Block(item1.start, min(item1.end, item2.end) - item1.start, (item1.status, item2.status))
        result.append(part)
        item1 = Block(part.end, item1.size - part.size, item1.status) if item1.size > part.size else next(iter1, None)
        item2 = Block(part.end, item2.size - part.size, item2.status) if item2.size > part.size else next(iter2, None)
    return result


def texture(rescue):
    '''create a 3D texture representing the rescue domain as a tree'''
    tex_size = 256

    # padding of rescue.blocks so that rescue.size is a multiple of tex_size
    levels = 0
    while rescue.sector_size * 256**levels < rescue.size:
        levels += 1
    logging.info(f'levels = {levels}')
    padding_size = rescue.sector_size * 256**levels-1 - rescue.size + 1  # TODO: check why +1 is needed
    logging.info(f'padding_size = {padding_size}')
    if padding_size > 0:
        padding = Block(rescue.end, padding_size, '')
        rescue.blocks.append(padding)

    # create an empty texture
    tex_shape = (tex_size,)*3+(4,)
    tex = np.zeros(shape=tex_shape).astype('u1')

    # create the list of blocks needed to get the first texture line
    def pixel_blocks(rescue, tex_size, levels):
        blocks = list()
        for x in range(tex_size):
            pixel = Block(
                start = rescue.start + x * rescue.sector_size * tex_size**(levels-1),
                size = rescue.sector_size * tex_size**(levels-1),
                status = ''
            )
            blocks.append(pixel)        
        return blocks

    blocks = pixel_blocks(rescue, tex_size, levels)

    first_texture_line = merge(rescue.blocks, blocks)
    print(first_texture_line)

    # fill the first texture line
    #for x, block in enumerate(first_texture_line):
    #    tex[x][0][0] = 

    return tex


if __name__ == '__main__':
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
    rescue = Rescue('../tests/Seagate1.mapfile')
    tex = texture(rescue)
    
    app = QtWidgets.QApplication(sys.argv)

    widget = Widget()
    widget.show()
    sys.exit(app.exec_())
