from PythonQt.QtGui import QKeyEvent
from PythonQt.QtCore import QElapsedTimer
from PythonQt.QtCore import Qt
from PythonQt.QtCore import QDateTime
from PythonQt.QtCore import QDate
from PythonQt.QtCore import QTime
from PythonQt.libplanet import Vector3
from math import exp
from math import log



class SpatialData:
    def __init__(self, cosmoPos, invscale, bodyName = '', systemName = ''):
        self.cosmoPos = cosmoPos
        self.scale = 1.0 / invscale
        self.bodyName = bodyName
        self.systemName = systemName

class TemporalData:
    def __init__(self, timeCoeff = 1.0):
        self.timeCoeff = timeCoeff

class UI:
    def __init__(self, luminosity=110000008.0, orbits=False, labels=False, darkmatter=False, grid=False):
        self.luminosity = luminosity
        self.orbits = orbits
        self.labels = labels
        self.darkmatter = darkmatter
        self.grid = grid

class Scene:
    def __init__(self, spatialData, temporalData = TemporalData(), ui = UI()):
        self.spatialData = spatialData
        self.temporalData = temporalData
        self.ui = ui


# interpolate functions between 0 and 1 with continuous parameter t from 0 to 1

def interpolateBool(b0, b1, t):
    if t < 0.5:
        return b0
    else:
        return b1

def interpolateLinear(x0, x1, t):
    return x0 * (1 - t) + x1 * t

def interpolateLog(x0, x1, t):
    return exp(log(x0) * (1 - t) + log(x1) * t)

# interpolatePlanetPos

def interpolateSpatialData(s0, s1, t):
    bn = s1.bodyName
    if bn == '':
        bn = s0.bodyName
    return SpatialData(
        interpolateLinear(s0.cosmoPos, s1.cosmoPos, t),
        1.0 / interpolateLog(s0.scale, s1.scale, t),
        bn,
        s1.systemName
    )

def interpolateTemporalData(t0, t1, t):
    return TemporalData(
        interpolateLog(t0.timeCoeff, t1.timeCoeff, t)
    )

def interpolateUI(ui0, ui1, t):
    return UI(
        interpolateLog(ui0.luminosity, ui1.luminosity, t),
        interpolateBool(ui0.orbits, ui1.orbits, t),
        interpolateBool(ui0.labels, ui1.labels, t),
        interpolateBool(ui0.darkmatter, ui1.darkmatter, t),
        interpolateBool(ui0.grid, ui1.grid, t)
    )

def interpolateScene(sc0, sc1, t):
    return Scene(
        interpolateSpatialData(sc0.spatialData, sc1.spatialData, t),
        interpolateTemporalData(sc0.temporalData, sc1.temporalData, t),
        interpolateUI(sc0.ui, sc1.ui, t)
    )
    

""" bad interactions :
0:
    2
    3
    4
    5

1: copy 0

2:
    4
    5

3: copy 2

"""

scenes = [
    # Earth
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 130000000, 'Earth'),
           TemporalData(), UI(0.167)),
    # Earth-Moon dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 300000000, 'Earth'),
          TemporalData(100000.0), UI(0.167, True, True)),
    # Saturn
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 130000000, 'Saturn'),
           TemporalData(), UI(0.167)),
    # Saturn moons dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 1300000000, 'Saturn'),
          TemporalData(100000.0), UI(0.167, True, True)),
    # inner Solar System dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 2.27987e+11, 'Sun'),
          TemporalData(1000000.0), UI(0.167, True, True)),
    # outer Solar System dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 5.65181e+12, 'Sun'),
          TemporalData(10000000.0), UI(0.167, True, True)),
    # Milky Way
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 6.171e+20),
           TemporalData(), UI(18.1, False, True)),
    # Local Group
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 2.469e+22),
           TemporalData(), UI(55800.0, False, True, True)),
    # Whole cube
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 8.7474e+23),
           TemporalData(), UI(1015000.0, False, False, True)),
]

prev_id = -1
id = 8

def keyPressEvent(e):
    global prev_id
    global timer
    global startScale
    global id

    # if spacebar pressed, start animation
    numpad_mod = int(e.modifiers()) == Qt.KeypadModifier
    if e.key() == Qt.Key_0 and numpad_mod:
        prev_id = id
        id = 0
        timer.restart()
    if e.key() == Qt.Key_1 and numpad_mod:
        prev_id = id
        id = 1
        timer.restart()
    if e.key() == Qt.Key_2 and numpad_mod:
        prev_id = id
        id = 2
        timer.restart()
    if e.key() == Qt.Key_3 and numpad_mod:
        prev_id = id
        id = 3
        timer.restart()
    if e.key() == Qt.Key_4 and numpad_mod:
        prev_id = id
        id = 4
        timer.restart()
    if e.key() == Qt.Key_5 and numpad_mod:
        prev_id = id
        id = 5
        timer.restart()
    if e.key() == Qt.Key_6 and numpad_mod:
        prev_id = id
        id = 6
        timer.restart()
    if e.key() == Qt.Key_7 and numpad_mod:
        prev_id = id
        id = 7
        timer.restart()
    if e.key() == Qt.Key_8 and numpad_mod:
        prev_id = id
        id = 8
        timer.restart()
    if e.key() == Qt.Key_9 and numpad_mod:
        prev_id = id
        id = 9
        timer.restart()
    if e.key() == Qt.Key_Space:
        id = -1

def initScene():
    global timer
    timer = QElapsedTimer()

def updateScene():
    global prev_id
    global id
    global timer
    if id not in range(len(scenes)):
        return

    t = timer.elapsed() / 10000.0
    if prev_id in range(len(scenes)) and t <= 1.0:
        scene=interpolateScene(scenes[prev_id], scenes[id], t)
    else:
        timer.invalidate()
        prev_id = -1
        scene=scenes[id]

    spatialData = scene.spatialData
    VIRUP.scale = spatialData.scale
    if spatialData.systemName != '':
        VIRUP.planetarySystemName = spatialData.systemName
    if spatialData.bodyName != '':
        if VIRUP.planetarySystemLoaded:
            VIRUP.planetTarget = spatialData.bodyName
            VIRUP.planetPosition = Vector3(0, 0, -1.125 / VIRUP.scale)
        else:
            VIRUP.cosmoPosition = spatialData.cosmoPos + Vector3(0, 0, -1.125*3.24078e-20 / VIRUP.scale)
    else:
        VIRUP.cosmoPosition = spatialData.cosmoPos + Vector3(0, 0, -1.125*3.24078e-20 / VIRUP.scale)

    temporalData = scene.temporalData
    VIRUP.timeCoeff = temporalData.timeCoeff

    ui = scene.ui
    VIRUP.cosmolum = ui.luminosity
    VIRUP.orbitsEnabled = ui.orbits
    VIRUP.labelsEnabled = ui.labels
    VIRUP.darkmatterEnabled = ui.darkmatter
    VIRUP.gridEnabled = ui.grid

