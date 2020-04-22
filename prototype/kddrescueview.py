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
from collections import namedtuple, defaultdict


class Scene:
    def __init__(self, ctx, tex, reserve='16MB'):
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

                uniform int pow2_tex_size;
                uniform int pow2_zoom_level;
                uniform ivec2 FragResolution;
                uniform float vertical_scrolling;
                uniform float rescue_domain_percentage;
                uniform int square_size;
                uniform usampler3D tex;
                int scroll_width = square_size;

                out vec4 gl_FragColor;

                vec4 color(uint status) {
                    // light grey for unknown status
                    if(status == 0u) return vec4(0.75, 0.75, 0.75, 1.0);

                    // compute the square color from a bitfield status
                    float nontried = float(status & 1u); 
                    float recovered = float(status & 2u); 
                    float nontrimmed = float(status & 4u);
                    float nonscraped = float(status & 8u) * 1.25;  // multiply by 1.25 to get 10 factor as in the original
                    float badsectors = float(status & 16u) * 2.5;  // multiply by 2.5 to get 40 factor as in the original
                    float colors = nontried + recovered + nontrimmed + nonscraped + badsectors; 
                    float red = (nontried * 64./255. + nontrimmed + nonscraped * 32./255. + badsectors) / colors;
                    float green = (nontried * 64./255. + nontrimmed * 224./255. + nonscraped * 32./255. + recovered) / colors;
                    float blue =  (nontried * 64./255. + nonscraped) / colors;
                    return vec4(red, green, blue, 1.);
                }

                
                void main() {
                    // stage 1: computes all resolutions
                    int TextureResolution = 1 << pow2_tex_size;  // = 2**pow2_tex_size
                    int SquareMaxResolution = 1 << pow2_zoom_level;  // = 2**pow2_zoom_level
                    //float SquareResolution = ceil(SquareMaxResolution * rescue_domain_percentage);
                    int SquareResolution = SquareMaxResolution;
                    int GridColumns = FragResolution.x / square_size;
                    int GridRows = int(ceil(SquareResolution / float(GridColumns)));
                    ivec2 GridResolution = ivec2(GridColumns, GridRows);
                    ivec2 CanvasResolution = ivec2(FragResolution.x, GridResolution.y * square_size);

                    // stage 2: computes all coordinates
                    ivec2 FragCoord = ivec2(gl_FragCoord.x, FragResolution.y - gl_FragCoord.y);
                    ivec2 CanvasCoord = ivec2(FragCoord.x, vertical_scrolling + FragCoord.y);
                    ivec2 GridCoord = CanvasCoord / square_size;
                    int SquareCoord = GridCoord.y * GridResolution.x + GridCoord.x;
/*
                    // stage 3: scroll bar with mini-map
                    if(FragCoord.x < scroll_width) {
                        // mini-map scroll bar
                        int scrollbar_top = int(vertical_scrolling);
                        int scrollbar_bottom = int(scrollbar_top + FragResolution.y * FragResolution.y / CanvasResolution.y);

                        if(FragCoord.y > scrollbar_top && FragCoord.y < scrollbar_top + 3) {
                            // scrollbar top 3 pixels high
                            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                            return;
                        }
                        if(FragCoord.y > scrollbar_bottom - 3 && FragCoord.y < scrollbar_bottom) {
                            // scrollbar bottom 3 pixels high
                            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                            return;
                        }
                        if(FragCoord.y > scrollbar_top && FragCoord.y < scrollbar_bottom && FragCoord.x < 2) {
                            // scrollbar left 2 pixels wide
                            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                            return;
                        }
                        if(FragCoord.y > scrollbar_top && FragCoord.y < scrollbar_bottom && FragCoord.x > scroll_width - 3) {
                            // scrollbar right 2 pixels wide
                            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                            return;
                        }

                        // else mini-map background
                        // We need indirections to have "2**n * rescue_domain_percentage texture pixels >= FragResolution.y" with n a multiple of pow2_tex_size
                        // n = int(ceil(ceil(log2(h/rescue_domain_percentage))/pow2_tex_size))*pow2_tex_size
                        
                        int indirections = 1;  // TODO: compute indirections from n
                        int PixelCoord = int(float(FragCoord.y) / float(FragResolution.y) * (1 << (pow2_tex_size * (indirections+1))) * rescue_domain_percentage);
                        ivec3 icoord;             // NOTE: texture needs z, y, x coordinates
                        uvec3 ucoord = uvec3(0);  // starts at texture line (0, 0)

                        for(int i = indirections; i >= 0; --i) {
                            icoord = ivec3(PixelCoord/(1 << (pow2_tex_size * i)) % TextureResolution, ucoord.y, ucoord.x);
                            ucoord = texelFetch(tex, icoord, 0).xyz;
                        }
                        // TODO: do not take the first pixel but multisample from the texture
                        gl_FragColor = color(ucoord.z);
                        return;
                    }
                    
                    // 0. We draw the scrolling cursor
                    // 1. How many pixel vertically
                    // 2. How many texels are needed (we need at least one pixel/texel)
                    // 3. Reach the righ depth in the texture
                    // 4. We will lookup at several float coord in the texture
                    // 5. We OR the lookup results
                    // draw status color for each line of the scroll bar (including the scroll cursor)
                    // that is: for each scroll bar pixel line, select the domain interval and lookup in the texture
*/


                    // stage 3: margins and grid bars
                    if(    CanvasCoord.x > CanvasResolution.x - mod(CanvasResolution.x, square_size)  // right margin as there is not enough space for full squares
                        || CanvasCoord.y > CanvasResolution.y - mod(CanvasResolution.y, square_size)  // bottom margin as there is not enough space for full squares
                        || mod(CanvasCoord.x, square_size) == 0                                  // vertical grid bars
                        || mod(CanvasCoord.y, square_size) == 0                                  // horizontal grid bars
                    ) {
                        gl_FragColor = vec4(1.);
                        return;
                    }
/*
                    // stage 3bis: test for a specific square number or texture pixel
                    if(SquareCoord == SquareResolution) {
                        gl_FragColor = vec4(1.0, 0.5, 0.5, 1.0);  // pink
                        //gl_FragColor = color(texelFetch(tex, ivec3(94, 63, 0), 0).z);
                        return;
                    }
*/
                    // stage 4: squares outside of rescue domain in almost grey 
                    if(SquareCoord >= SquareResolution) {
                        gl_FragColor = vec4(0.95);
                        return;
                    }

                    // stage 5: (SquareCoord) -> Statuses from Texture
                    ivec3 icoord;             // NOTE: texture needs z, y, x coordinates
                    uvec3 ucoord = uvec3(0);  // starts at texture line (0, 0)
                    int indirections = pow2_zoom_level / pow2_tex_size;
                    int pow2_pixel_range = pow2_tex_size - pow2_zoom_level % pow2_tex_size;
                    int pixel_range = 1 << pow2_pixel_range;
                    int PixelCoord = SquareCoord * pixel_range;
                    uint statuses = 0u;

                    for(int i = indirections; i >= 0; --i) {
                        if(i != 0) {
                            icoord = ivec3(PixelCoord/(1 << (pow2_tex_size * i)) % TextureResolution, ucoord.y, ucoord.x);
                            ucoord = texelFetch(tex, icoord, 0).xyz;
                        } else {
                            for(int j = 0; j < pixel_range; ++j) {
                                icoord = ivec3((PixelCoord+j)/(1 << (pow2_tex_size * i)) % TextureResolution, ucoord.y, ucoord.x);
                                statuses |= texelFetch(tex, icoord, 0).z;
                            }
                        }
                    }

                    // dummy use of uniforms
                    for(float i = 0.0; i < vertical_scrolling; ++i) { }
                    for(float i = 0.0; i < rescue_domain_percentage; ++i) { }
                    
                    // stage 6: Statuses from texture -> Square color
                    gl_FragColor = color(statuses);
                }
            ''',
        )

        self.vbo = ctx.buffer(reserve='4MB', dynamic=True)
        self.vao = ctx.simple_vertex_array(self.prog, self.vbo, 'vertices')

        tex_data, pow2_tex_size, rescue_domain_percentage = tex
        tex_size = 2**pow2_tex_size
        self.prog['pow2_tex_size'] = pow2_tex_size
        self.tex = ctx.texture3d(size=(tex_size,)*3, components=3, data=tex_data, alignment=1, dtype='u1')
        self.tex.filter = (moderngl.NEAREST, moderngl.NEAREST)
        self.tex.use()
        self.prog['FragResolution'] = (512, 512)
        self.prog['vertical_scrolling'] = 0.0
        self.prog['rescue_domain_percentage'] = rescue_domain_percentage
        self.prog['square_size'] = 8
        self.prog['pow2_zoom_level'] = 16

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
    def __init__(self, tex):
        fmt = QtOpenGL.QGLFormat()
        fmt.setVersion(3, 3)
        fmt.setProfile(QtOpenGL.QGLFormat.CoreProfile)
        fmt.setSampleBuffers(True)
        self.timer = QtCore.QElapsedTimer()        
        super(Widget, self).__init__(fmt, None)
        self.scene = None
        self.tex = tex

    def paintGL(self):
        self.ctx = moderngl.create_context()
        self.screen = self.ctx.detect_framebuffer()
        self.init()
        self.render()
        self.paintGL = self.render

    def init(self):
        self.resize(512, 512)
        self.ctx.viewport = (0, 0, 512, 512)
        self.scene = Scene(self.ctx, self.tex)

    def render(self):
        self.screen.use()
        self.scene.clear()
        self.scene.plot(vertices)

    def wheelEvent(self, event):
        steps = event.angleDelta().y() / 8 / 15.0
        if event.modifiers() == QtCore.Qt.ControlModifier:
            # zoom
##            zoom_factor = self.scene.prog['zoom_factor'].value * (1.0 - steps / 50.)
##            zoom_factor = 1.0 if zoom_factor < 1.0 else zoom_factor
##            self.scene.prog['zoom_factor'] = zoom_factor
##            logging.info(f'zoom_factor = {zoom_factor}')
            # TODO: correct vertical position to stay on the canvas
            pow2_zoom_level = self.scene.prog['pow2_zoom_level'].value - int(steps)
            pow2_zoom_level = 1 if pow2_zoom_level < 1 else 31 if pow2_zoom_level > 31 else pow2_zoom_level
            logging.info(f'pow2_zoom_level = {pow2_zoom_level}')
            self.prog['pow2_zoom_level'] = int(pow2_zoom_level)   
            # TODO: correct vertical position to stay on the canvas
        else:
            # scroll
            v_pos = self.scene.prog['vertical_scrolling'].value - steps * 5  # scroll by 5 pixels
            #v_pos_max = self.scene.prog['zoom_factor'].value - 1.0
            #v_pos = 0.0 if v_pos < 0.0 else v_pos_max if v_pos > v_pos_max else v_pos
            v_pos = 0.0 if v_pos < 0.0 else v_pos
            self.scene.prog['vertical_scrolling'] = v_pos
            logging.info(f'vertical_scrolling = {v_pos}')
        event.accept()
        self.update()


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
    '': 'unknown status',
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


def color(statuses):
    '''computes a pixel bitfield color from rescue block statuses'''
    statuses = set(statuses)
    if not statuses:
        return 0
    return sum(binary_statuses.get(status, 0) for status in statuses)


def texture(rescue):
    '''create a 3D texture representing the rescue domain as a tree'''

    pow2_tex_size = 8            # guaranteed up to 8:
    tex_size = 2**pow2_tex_size  # OpenGl 3.3 supports at least 256*256*256 textures
 
    # create an empty texture
    tex_shape = (tex_size,)*3+(3,)
    tex = np.zeros(shape=tex_shape).astype('u1')

    # iterator for texture lines
    lines = itertools.product(range(tex_size), repeat=2)

    # create a memory of texture lines with a single status
    def fill_single_status_line(status):
         x, y, bitfield = next(lines) + (color(status),)
         for z in range(tex_size):
             tex[x][y][z] = (x, y, bitfield)
         return (x, y, bitfield)
        
    class status_memory(defaultdict):
        def __missing__(self, status_key):
            if self.default_factory is None:
                raise KeyError(status_key)
            result = self[status_key] = self.default_factory(status_key)
            return result
        
    memory = status_memory(fill_single_status_line)


    # compute the number of tree levels stored recursively in the texture 
    texture_tree_depth = int(np.ceil(np.log2(rescue.size/rescue.sector_size)/pow2_tex_size))
    logging.info(f'texture_tree_depth = {texture_tree_depth}')

    # the texture represents sector_size * tex_size**(levels+1) bytes
    # so rescue.blocks needs padding to match this size 
    padding_size = rescue.sector_size * tex_size**texture_tree_depth - rescue.size
    logging.info(f'padding_size = {padding_size:_}')
    # percentage of the rescue domain which is meaningful
    rescue_domain_percentage = rescue.size / (rescue.size + padding_size)
    logging.info(f'percentage of meaningful rescue domain = {rescue_domain_percentage}')
    if padding_size > 0:
        padding = Block(rescue.end, padding_size, '')
        rescue.blocks.append(padding)

    # iterator for rescue blocks
    blocks = iter(rescue.blocks)
    block = next(blocks, None)

    def populate_line(line_block):
        '''fill a texture line corresponding to a block'''
        # create the blocks corresponding to the line pixels
        pixel_blocks = list()
        pixel_size = int(line_block.size / tex_size)
        for i in range(tex_size):
            pixel_start = line_block.start + i * pixel_size
            pixel_block = Block(pixel_start, pixel_size, '')
            pixel_blocks.append(pixel_block)
        pixels = iter(pixel_blocks)
        pixel = next(pixels, None)

        # access existing block
        nonlocal block

        # initialise the line statuses
        line_statuses = ''

        # select an empty texture line
        x, y = next(lines)
        z = 0

        while block and pixel:
            if block.end > pixel.end:
                tex[x][y][z] = memory[block.status]
                line_statuses += block.status
                z += 1
                block = Block(pixel.end, block.size - pixel_size, block.status)
                pixel = next(pixels, None)
                continue
                
            if block.end == pixel.end:
                tex[x][y][z] = memory[block.status]
                line_statuses += block.status
                z += 1
                block = next(blocks, None)
                pixel = next(pixels, None)
                continue
            
            if block.end < pixel.end:
                pixel_x, pixel_y, pixel_statuses = populate_line(pixel)
                tex[x][y][z] = (pixel_x, pixel_y, color(pixel_statuses))
                line_statuses += pixel_statuses
                z += 1
                pixel = next(pixels, None)
                continue
        
        return (x, y, line_statuses)

    first_line = Block(rescue.start, rescue.size, '')
    populate_line(first_line)

    # print the meaningful part of the first texture line for debugging
    print(tex[0][0][:int(np.ceil(tex_size * rescue_domain_percentage))])

    return (tex, pow2_tex_size, rescue_domain_percentage)


if __name__ == '__main__':
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
    rescue = Rescue('../tests/example.log')
    #rescue = Rescue('../tests/Seagate1.mapfile')
    tex = texture(rescue)
    
    app = QtWidgets.QApplication(sys.argv)
    widget = Widget(tex)
    widget.show()
    sys.exit(app.exec_())
