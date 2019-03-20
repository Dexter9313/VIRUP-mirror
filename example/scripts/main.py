from PythonQt.QtGui import QVector3D
from PythonQt.QtGui import QColor
from PythonQt.QtCore import Qt
import math
from time import time

def initScene():
    global mesh
    global shader
    global angle
    global t
    global t0

    t0 = time()
    t = 0

    angle = 0
    mesh = GLHandler.newMesh()
    shader = GLHandler.newShader("default")
    GLHandler.setShaderParam(shader, "alpha", 1.0)
    GLHandler.setShaderParam(shader, "color", QColor(128, 255, 255))

    GLHandler.setVertices(mesh, [0.5, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.5], shader, ["position"], [3], [0, 1, 1, 2, 2, 0])

    HydrogenVR.appendPostProcessingShader("py", "grayscale")


def updateScene():
    global mesh
    global shader
    global angle
    global t
    dt = time() - t - t0
    t += dt

    angle = t*math.pi/2
    camera.lookAt(QVector3D(math.cos(angle), math.sin(angle), 1), QVector3D(0,0,0), QVector3D(0,0,1))

def renderScene():
    global mesh
    global shader

    GLHandler.setUpRender(shader)
    GLHandler.render(mesh, GLHandler.LINES)

def cleanUpScene():
    global mesh
    global shader

    GLHandler.deleteMesh(mesh)
    GLHandler.deleteShader(shader)

def applyPostProcShaderParams(ppid, shader):
    if ppid == "py":
        GLHandler.setShaderParam(shader, "lum", 0.5)

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
