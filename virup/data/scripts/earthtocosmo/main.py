from PythonQt.QtGui import QKeyEvent
from PythonQt.QtCore import QElapsedTimer
from PythonQt.QtCore import Qt
from PythonQt.QtCore import QDateTime
from PythonQt.QtCore import QDate
from PythonQt.QtCore import QTime
from PythonQt.libplanet import Vector3
from math import exp
from math import log

def keyPressEvent(e):
    global timer
    global startScale

    # if spacebar pressed, start animation
    if e.key() == Qt.Key_Space:
        timer.restart()

def initScene():
    global timer
    global startScale
    global tgtScale
    global i

    i = 0

    # animation timer
    timer = QElapsedTimer()
    startScale = 7.69230769e-8 #earth diameter = 1m
    tgtScale = 1.0 / 3.08568e+25 #~ 1:1 Gpc scale

def updateScene():
    global timer
    global tgtScale
    global startScale
    global i
    global initCosmoPos

    # right now it's not possible to go to the solar system and somewhere else inside it during the same frame
    # if it was possible this code should belong to initScene()
    i += 1
    if i == 1:
        # go to solar eclipse July 2nd 2019 time
        d=QDate(2019, 7, 2)
        t=QTime(19, 29, 42)
        VIRUP.simulationTime = QDateTime(d, t, Qt.UTC)
        # go to solar system
        VIRUP.cosmoPosition = Vector3(8.29995608, 0.0, -0.027)
        VIRUP.scale = startScale
    if i == 2:
        # go to Earth where eclipse is visible
        # (Coordinates found by moving camera (WASD translation - mousewheel for scale/speed),
        # then use python console (F8) : "print(VIRUP.planetPosition)")
        VIRUP.planetPosition = Vector3(0, 0, 0)
        VIRUP.planetTarget = 'Earth'
        initCosmoPos = VIRUP.cosmoPosition
        VIRUP.planetPosition = Vector3(0, 0, -1.125 / VIRUP.scale)

    # if animation is running
    if timer.isValid():
        # make it last 240 seconds (t from 0 to 1 first part, 1 to 2 second part)
        t = timer.elapsed() / 120000.0

        # if animation is over
        if t > 2.0:
            VIRUP.scale = startScale
            timer.invalidate()
        # elif second part
        elif t > 1.0:
            # scale linearly through log space
            VIRUP.scale = exp(log(tgtScale) * (2 - t) + log(startScale) * (t-1))
        # else; first part
        else:
            # scale linearly through log space
            VIRUP.scale = exp(log(startScale) * (1 - t) + log(tgtScale) * t)

        VIRUP.cosmoPosition = initCosmoPos + Vector3(0, 0, -1.125*3.24078e-20 / VIRUP.scale)


