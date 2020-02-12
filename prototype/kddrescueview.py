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


import sys
import numpy as np
from PyQt5 import QtWidgets, QtCore, QtOpenGL
import moderngl

class QModernGLWidget(QtOpenGL.QGLWidget):
    def __init__(self):
        fmt = QtOpenGL.QGLFormat()
        fmt.setVersion(3, 3)
        fmt.setProfile(QtOpenGL.QGLFormat.CoreProfile)
        fmt.setSampleBuffers(True)
        self.timer = QtCore.QElapsedTimer()
        super(QModernGLWidget, self).__init__(fmt, None)

    def initializeGL(self):
        pass

    def paintGL(self):
        self.ctx = moderngl.create_context()
        self.screen = self.ctx.detect_framebuffer()
        self.init()
        self.render()
        self.paintGL = self.render

    def init(self):
        pass

    def render(self):
        pass


class HelloWorld2D:
    def __init__(self, ctx, reserve='4MB'):
        self.ctx = ctx
        self.prog = self.ctx.program(
            vertex_shader='''
                #version 330

                in vec2 in_vert;
                in vec4 in_color;
                out vec4 v_color;

                void main() {
                    v_color = in_color;
                    gl_Position = vec4(in_vert, 0.0, 1.0);
                }
            ''',
            fragment_shader='''
                #version 330
                
                in vec4 v_color;
                out vec4 f_color;

                void main() {
                    f_color = v_color;
                }
            ''',
        )

        self.vbo = ctx.buffer(reserve='4MB', dynamic=True)
        self.vao = ctx.simple_vertex_array(self.prog, self.vbo, 'in_vert', 'in_color')

    def clear(self, color=(0, 0, 0, 0)):
        self.ctx.clear(*color)

    def plot(self, points):
        data = points.astype('f4').tobytes()
        self.vbo.orphan()
        self.vbo.write(data)
        self.vao.render(moderngl.TRIANGLES, vertices=len(data))


verts = np.array(
    # upper left triangle  bottom right triangle
    [ [-1.0, +1.0, -1.0,   +1.0, -1.0, +1.0],  # x
      [+1.0, +1.0, -1.0,   +1.0, -1.0, -1.0],  # y
      [ 1.0,  1.0,  1.0,    1.0,  1.0,  1.0],  # r
      [ 0.0,  0.0,  0.0,    0.0,  0.0,  0.0],  # g
      [ 0.0,  0.0,  0.0,    0.0,  0.0,  0.0],  # b
      [ 1.0,  1.0,  1.0,    1.0,  1.0,  1.0],  # a
    ]).T  # transpose for OpenGL

class MyWidget(QModernGLWidget):
    def __init__(self):
        super(MyWidget, self).__init__()
        self.scene = None

    def init(self):
        self.resize(512, 512)
        self.ctx.viewport = (0, 0, 512, 512)
        self.scene = HelloWorld2D(self.ctx)

    def render(self):
        self.screen.use()
        self.scene.clear()
        self.scene.plot(verts)


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    widget = MyWidget()
    widget.show()
    sys.exit(app.exec_())
