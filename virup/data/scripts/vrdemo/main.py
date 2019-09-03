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
    def __init__(self, cosmoPos, invscale, bodyName = '', systemName = '', planetPos = Vector3()):
        self.cosmoPos = cosmoPos
        self.scale = 1.0 / invscale
        self.bodyName = bodyName
        self.systemName = systemName
        self.planetPos = planetPos

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

def interpolateSpatialData(s0, s1, t):
    global longanimation

    longanimation = False
    planetpos = Vector3()
    scale=1.0 / interpolateLog(s0.scale, s1.scale, t)

    if s0.systemName != '' and s1.systemName != '' and s0.systemName != s1.systemName:
        dist=(s0.cosmoPos - s1.cosmoPos).length() * 3.086e+19
        if t <= 0.25:
            inter0=SpatialData(s0.cosmoPos, dist)
            result=interpolateSpatialData(s0, inter0, t*4)
        elif t <= 0.75:
            inter0=SpatialData(s0.cosmoPos, dist)
            inter1=SpatialData(s1.cosmoPos, dist)
            result=interpolateSpatialData(inter0, inter1,t*2 - 0.5)
        else:
            inter1=SpatialData(s1.cosmoPos, dist)
            result=interpolateSpatialData(inter1, s1, t*4 - 3)
        longanimation = True
        return result

    if s0.bodyName != '' and s1.bodyName != '' and s0.bodyName != s1.bodyName :
        longanimation = True
        bn = VIRUP.getClosestCommonAncestorName(s0.bodyName, s1.bodyName)
        planetpos = VIRUP.interpolateCoordinates(s0.bodyName, s1.bodyName, t)

        dist = (VIRUP.interpolateCoordinates(s0.bodyName, s1.bodyName, 0) - VIRUP.interpolateCoordinates(s0.bodyName, s1.bodyName, 1)).length()
        maxscale = 1.0 / (dist)
        if t <= 0.25:
            scale = 1.0 / interpolateLog(s0.scale, maxscale, 4*t)
            planetpos = VIRUP.interpolateCoordinates(s0.bodyName, s1.bodyName, 0)
        elif t <= 0.75:
            scale = 1.0 / maxscale
            # maybe try some smoother t
            tprime = t*2 - 0.5
            planetpos = VIRUP.interpolateCoordinates(s0.bodyName, s1.bodyName, tprime)
        else:
            scale = 1.0 / interpolateLog(maxscale, s1.scale, 4*t - 3)
            planetpos = VIRUP.interpolateCoordinates(s0.bodyName, s1.bodyName, 1)
    else:
        bn = s1.bodyName
        if bn == '':
            bn = s0.bodyName

    return SpatialData(
        interpolateLinear(s0.cosmoPos, s1.cosmoPos, t),
        scale,
        bn,
        s1.systemName,
        planetpos
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

scenes = [
    # Earth
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 130000000, 'Earth', 'Solar System'),
           TemporalData(), UI(0.167)),
    # Earth-Moon dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 300000000, 'Earth',  'Solar System'),
          TemporalData(100000.0), UI(0.167, True, True)),
    # Saturn
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 130000000, 'Saturn', 'Solar System'),
           TemporalData(), UI(0.167)),
    # Saturn moons dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 1300000000, 'Saturn', 'Solar System'),
          TemporalData(100000.0), UI(0.167, True, True)),
    # inner Solar System dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 2.27987e+11, 'Sun', 'Solar System'),
          TemporalData(1000000.0), UI(0.167, True, True)),
    # outer Solar System dynamics
    Scene(SpatialData(Vector3(8.29995608, 0.0, -0.027), 5.65181e+12, 'Sun', 'Solar System'),
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
    # Kepler-11 general area
    Scene(SpatialData(Vector3(8.094034192480557, -0.5269932753083851, -0.13332218244029664), 5e+10, 'Kepler-11', 'Kepler-11'),
           TemporalData(50000), UI(0.167, True, True)),
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
    global longanimation

    timer = QElapsedTimer()
    longanimation = False

def updateScene():
    global prev_id
    global id
    global timer
    global longanimation
    if id not in range(len(scenes)):
        return

    if longanimation:
        t = timer.elapsed() / 15000.0
    else:
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

    if spatialData.bodyName != '' and VIRUP.planetarySystemLoaded:
        VIRUP.planetTarget = spatialData.bodyName
        VIRUP.planetPosition = spatialData.planetPos + Vector3(0, 0, -1.125 / VIRUP.scale)
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

