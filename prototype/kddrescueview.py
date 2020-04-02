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
                out vec2 v_text;

                void main() {
                    gl_Position = vec4(vertices, 0.0, 1.0);
                    v_text = vertices;
                }
            ''',
            fragment_shader='''
                #version 330

                uniform int tex_size;
                uniform usampler3D tex;

                in vec2 v_text;
                out vec4 gl_FragColor;

                void main() {
                    vec3 coord = vec3(v_text/2. + .5, 0.) * vec3(float(tex_size));
                    ivec3 icoord0 = ivec3(coord);
                    uvec3 rgb0 = texelFetch(tex, icoord0, 0).rgb;
                    ivec3 icoord1 = ivec3(rgb0);
                    uvec3 rgb1 = texelFetch(tex, icoord1, 0).rgb;
                    vec3 rgb = vec3(rgb1) / vec3(tex_size);
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
Block = namedtuple('Block', 'position size status')

class Rescue:
    def __init__(self, mapfile):
        self.command = None
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

            if self.command and line.startswith('# Command line:'):
                logging.error(f'line {n}: more than one command line')
                continue

            if line.startswith('# Command line:'):
                logging.debug(f'line {n}: command line')
                self.command = line
                match = re.search(r'(-b|--block-size=)\s*(?P<blocksize>[0-9]+)', line)
                self.sector_size = int(match.group('blocksize')) if match else 512  # TODO: if conversion fails
                logging.info(f'Sector size = {self.sector_size}')
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
                    position = int(parts[0], 0)
                    size = int(parts[1], 0)
                    status = parts[2].strip()
                except ValueError:
                    logging.debug(f'line {n}: not a block line')
                else:
                    if status not in block_statuses or position < 0 or size <= 0:
                        logging.error(f'line {n}: incorrect block line ({line})')
                        continue
                    logging.debug(f'line {n}: block line')
                    self.blocks.append(Block(position, size, status))
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

    def start(self):
        return self.blocks[0].position

    def end(self):
        return self.block[-1].position + self.blocks[-1].size
    
    def size(self):
        return self.end() - self.start()


if __name__ == '__main__':
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
    app = QtWidgets.QApplication(sys.argv)
    rescue = Rescue('../tests/Seagate1.mapfile')
    widget = Widget()
    widget.show()
    sys.exit(app.exec_())
