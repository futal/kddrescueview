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


# Template from moderngl/examples/window_pyqt5.py and imported files
#
# MIT License
#
# Copyright (c) 2017-2020 Szabolcs Dombi
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.


import numpy as np
import sys
from PyQt5 import QtWidgets
import moderngl
from PyQt5 import QtGui, QtWidgets, QtCore, QtOpenGL


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
                uniform vec2 Pan;
                in vec2 in_vert;
                in vec4 in_color;
                out vec4 v_color;
                void main() {
                    v_color = in_color;
                    gl_Position = vec4(in_vert - Pan, 0.0, 1.0);
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

    def pan(self, pos):
        self.prog['Pan'].value = pos

    def clear(self, color=(0, 0, 0, 0)):
        self.ctx.clear(*color)

    def plot(self, points, type='line'):
        data = points.astype('f4').tobytes()
        self.vbo.orphan()
        self.vbo.write(data)
        if type == 'line':
            self.ctx.line_width = 1.0
            self.vao.render(moderngl.LINE_STRIP, vertices=len(data) // 24)
        if type == 'points':
            self.ctx.point_size = 3.0
            self.vao.render(moderngl.POINTS, vertices=len(data) // 24)


class PanTool:
    def __init__(self):
        self.total_x = 0.0
        self.total_y = 0.0
        self.start_x = 0.0
        self.start_y = 0.0
        self.delta_x = 0.0
        self.delta_y = 0.0
        self.drag = False

    def start_drag(self, x, y):
        self.start_x = x
        self.start_y = y
        self.drag = True

    def dragging(self, x, y):
        if self.drag:
            self.delta_x = (x - self.start_x) * 2.0
            self.delta_y = (y - self.start_y) * 2.0

    def stop_drag(self, x, y):
        if self.drag:
            self.dragging(x, y)
            self.total_x -= self.delta_x
            self.total_y += self.delta_y
            self.delta_x = 0.0
            self.delta_y = 0.0
            self.drag = False

    @property
    def value(self):
        return (self.total_x - self.delta_x, self.total_y + self.delta_y)


def vertices():
    x = np.linspace(-1.0, 1.0, 50)
    y = np.random.rand(50) - 0.5
    r = np.ones(50)
    g = np.zeros(50)
    b = np.zeros(50)
    a = np.ones(50)
    return np.dstack([x, y, r, g, b, a])


verts = vertices()
pan_tool = PanTool()


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

    def mousePressEvent(self, evt):
        pan_tool.start_drag(evt.x() / 512, evt.y() / 512)
        self.scene.pan(pan_tool.value)
        self.update()

    def mouseMoveEvent(self, evt):
        pan_tool.dragging(evt.x() / 512, evt.y() / 512)
        self.scene.pan(pan_tool.value)
        self.update()

    def mouseReleaseEvent(self, evt):
        pan_tool.stop_drag(evt.x() / 512, evt.y() / 512)
        self.scene.pan(pan_tool.value)
        self.update()


app = QtWidgets.QApplication(sys.argv)
widget = MyWidget()
widget.show()
sys.exit(app.exec_())
