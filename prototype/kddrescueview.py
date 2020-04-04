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
import itertools
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

                ivec3 color(int status) {
                    // compute a color from a bitfield status
                    int nontried = status & 1; 
                    int recovered = status & 2; 
                    int nontrimmed = status & 4;
                    int nonscraped = status & 8;
                    int badsectors = status & 16; 
                    int colors = nontried + recovered + nontrimmed + nonscraped + badsectors;
                    int red = (nontried * 0x40 + nontrimmed * 0xff + nonscraped * 0x20 + badsectors * 0xff) / colors;
                    int green = (nontried * 0x40 + nontrimmed * 0xe0 + nonscraped * 0x20 + recovered * 0xff) / colors;
                    int blue =  (nontried * 0x40 + nonscraped * 0xff) / colors;
                    return ivec3(red, green, blue); 
                }
                
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
                    
                    vec3 rgb = vec3(color(int(ucoord.z))) / vec3(tex_size);
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
binary_statuses = {
    '?': 1,
    '+': 2,
    '*': 4,
    '/': 8,  # 10 as a color multiplicator in the original
    '-': 16, # 40 as a color multiplicator in the original
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
                    status = parts[1].strip()
                except:
                    position = None
                if position and status in rescue_statuses: 
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

def color(statuses):
    '''computes a pixel bitfield color from rescue block statuses'''
    return sum(binary_statuses.get(status, 0) for status in set(statuses))


def texture(rescue):
    '''create a 3D texture representing the rescue domain as a tree'''

    tex_size = 256  # 2**10 = 256 (see levels computation where 10 is used), OpenGl 3.3 requires 3D textures to support at least 256*256*256 textures

    # create an empty texture
    tex_shape = (tex_size,)*3+(3,)
    tex = np.zeros(shape=tex_shape).astype('u1')

    # iterator for texture lines
    tex_lines = itertools.product(range(tex_size), repeat=2)

    # save coordinates of the first texture line to be used as an entry point
    x0, y0 = next(tex_lines)
    
    # create a memory of texture lines with a single status TODO: create on demand with a defaultdict
    memory = dict()
    for status, bitfield in binary_statuses.items():
        x, y = next(tex_lines)
        for z in range(tex_size):
            tex[x][y][z] = (x, y, bitfield)
        memory[status] = (x, y, bitfield)


    logging.info(f'rescue.start = {rescue.start:_}')
    logging.info(f'rescue.size = {rescue.size:_}')

    # compute the number of tree levels stored recursively in the texture 
    levels = int(np.ceil(np.log2(rescue.size/rescue.sector_size)/10))  # 10 comes from 256 = 2**10
    logging.info(f'levels = {levels}')
    # at level 0, each pixel represents 1 sector (256**0)
    # at level 1, each pixel represents 256 sectors (256**1)
    # at level 2, each pixel represents 65_536 sectors (256**2)
    # at level 3, each pixel represents 16_777_216 sectors
    # at level 4, each pixel represents 4_294_967_296 sectors
    # ...
    # at level n, each pixel represents 256**n sectors    

    # the texture represents sector_size * tex_size**(levels+1) bytes
    # so rescue.blocks needs padding to match this size 
    padding_size = rescue.sector_size * tex_size**(levels+1) - rescue.size
    logging.info(f'padding_size = {padding_size:_}')
    if padding_size > 0:
        padding = Block(rescue.end, padding_size, '')
        rescue.blocks.append(padding)

    # iterator for rescue blocks
    rescue_blocks = iter(rescue.blocks)
    rescue_block = next(rescue_blocks)
    logging.info(f'initial rescue_block = {rescue_block}')

    # create the list of blocks needed to get the first texture line
    def pixel_blocks(rescue, tex_size, levels):
        blocks = list()
        for x in range(tex_size):
            pixel = Block(
                start = rescue.start + x * rescue.sector_size * tex_size**levels,
                size = rescue.sector_size * tex_size**levels,
                status = ''
            )
            blocks.append(pixel)        
        return blocks

    blocks = pixel_blocks(rescue, tex_size, levels)

    first_texture_line = merge(rescue.blocks, blocks)

    # fill the first texture line
    #for z, block in enumerate(first_texture_line):
    #    tex[0][0][z] = 

    return tex


if __name__ == '__main__':
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
    rescue = Rescue('../tests/example.log')
    tex = texture(rescue)
    
    app = QtWidgets.QApplication(sys.argv)

    widget = Widget()
    widget.show()
    sys.exit(app.exec_())
