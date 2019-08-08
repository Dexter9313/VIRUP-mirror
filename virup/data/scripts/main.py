from PythonQt.QtGui import QKeyEvent
from PythonQt.QtCore import QElapsedTimer
from PythonQt.QtCore import Qt
from math import exp
from math import log

def initScene():
    global timer
    global tgtScale
    global startScale

    timer = QElapsedTimer()
    tgtScale = 1.0 / 3.08568e+24 #~ 1 / 100 Mpc

def updateScene():
    global timer
    global tgtScale
    global startScale

    if timer.isValid():
        t = timer.elapsed() / 120000.0

        if t > 2.0:
            VIRUP.scale = startScale
            timer.invalidate()
        elif t > 1.0:
            VIRUP.scale = exp(log(tgtScale) * (2 - t) + log(startScale) * (t-1))
        else:
            VIRUP.scale = exp(log(startScale) * (1 - t) + log(tgtScale) * t)


def keyPressEvent(e):
    global timer
    global startScale

    if e.key() == Qt.Key_Space:
        timer.restart()
        startScale = VIRUP.scale

