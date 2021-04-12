import math
from time import time

from PythonQt.QtGui import QVector3D
from PythonQt.QtGui import QColor
from PythonQt.QtGui import QKeyEvent
from PythonQt.QtCore import Qt

from PythonQt.GL import GLShaderProgram
from PythonQt.GL import GLMesh

def initScene():
    global mesh
    global shader
    global angle
    global t
    global t0

    t0 = time()
    t = 0

    angle = 0
    mesh = GLMesh()
    shader = GLShaderProgram("default")
    shader.setUniform("alpha", 1.0)
    shader.setUniform("color", QColor(128, 255, 255))

    mesh.setVertexShaderMapping(shader, ["position"], [3])
    mesh.setVertices([0.5, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.5], [0, 1, 1, 2, 2, 0])

    #HydrogenVR.appendPostProcessingShader("py", "grayscale")


def updateScene():
    global mesh
    global shader
    global angle
    global t
    dt = time() - t - t0
    t += dt

    angle = t*math.pi/2
    #camera.lookAt(QVector3D(math.cos(angle), math.sin(angle), 1), QVector3D(0,0,0), QVector3D(0,0,1))

def renderScene():
    global mesh
    global shader

    GLHandler.setUpRender(shader)
    mesh.render(PrimitiveType.LINES)

def cleanUpScene():
    global mesh
    global shader

def applyPostProcShaderParams(ppid, shader):
    if ppid == "py":
        shader.setUniform("lum", 0.5)

def keyPressEvent(e):
    if e.key() == Qt.Key_A:
        print("A was pressed !")

def keyReleaseEvent(e):
    if e.key() == Qt.Key_A:
        print("A was released !")

def vrEvent(etype, side, button):
    if etype == VRHandler.BUTTON_PRESSED and side == Side.LEFT and button == VRHandler.MENU:
        print("Left menu button was pressed ! Its position is :")
        print(leftController.getPosition())
