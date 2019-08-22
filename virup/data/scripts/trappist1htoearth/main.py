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
    global startCosmoPos

    # if spacebar pressed, start animation
    if e.key() == Qt.Key_Space:
        timer.restart()
        startCosmoPos = VIRUP.cosmoPosition

def initScene():
    global timer
    global tgtCosmoPos
    global i

    i = 0

    # animation timer
    timer = QElapsedTimer()
    # earth cosmo position
    tgtCosmoPos = Vector3(8.299956322603713, -4.848213947459715e-09, -0.027000010013278848)

def updateScene():
    global timer
    global tgtCosmoPos
    global tgtEarthPos
    global startCosmoPos
    global i

    # right now it's not possible to do all this in the same frame
    # if it was possible this code should belong to initScene()
    i += 1
    if i == 1:
        # go to solar eclipse July 2nd 2019 time
        d=QDate(2019, 7, 2)
        t=QTime(19, 29, 42)
        VIRUP.simulationTime = QDateTime(d, t, Qt.UTC)
        # go to TRAPPIST-1 (fictional position)
        VIRUP.cosmoPosition = Vector3(1.18643, 0.0837579, 1.25077)
        # set 1:1000km scale
        VIRUP.scale = 1.0e-6
        # look toward solar system
        VIRUP.camYaw = 3.194538
        VIRUP.camPitch = -0.272878
    if i == 2:
        # set exoplanetary system to load
        VIRUP.planetarySystemName = "TRAPPIST-1"
    if i == 3:
        # go to TRAPPIST-1 h
        # (Coordinates found by moving camera (WASD translation - mousewheel for scale/speed),
        # then use python console (F8) : "print(VIRUP.planetPosition)")
        VIRUP.planetTarget = 'TRAPPIST-1 h'
        VIRUP.planetPosition = Vector3(-6.63215e06, 9.37568e06, 6.89158e06)


    # if animation is running
    if timer.isValid():
        # set animation duration
        duration = 120.0 # seconds
        # total distance traveled
        distance = (tgtCosmoPos - startCosmoPos).length()
        # acceleration needed to cover distance within duration with constant accel and decel
        accel = 4*distance/(duration*duration)

        # convert elapsed time to seconds
        t = timer.elapsed() / 1000.0

        # if animation is over
        if t > duration:
            VIRUP.cosmoPosition = tgtCosmoPos
            timer.invalidate()
        else:
            #accel
            if t < duration / 2.0:
                x= accel * t * t / 2.0
            #decel
            else:
                t -= duration / 2.0
                x = (-1*accel * t * t / 2.0) + (accel*t*duration/2.0) + (distance/2.0)
            # convert from 0 to 1
            x /= distance
            VIRUP.cosmoPosition = startCosmoPos * (1 - x) + tgtCosmoPos * x


