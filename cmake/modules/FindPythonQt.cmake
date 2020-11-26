# Find PythonQt
#
# Sets PYTHONQT_FOUND, PYTHONQT_INCLUDE_DIRS, PYTHONQT_LIBRARY, PYTHONQT_LIBRARIES
#

# Python is required
find_package(PythonLibs)
if(NOT PYTHONLIBS_FOUND)
  message(FATAL_ERROR "error: Python is required to build PythonQt")
endif()

# Python2
find_path(PYTHONQT_PYTHON2_INSTALL_DIR NAMES include/PythonQt/PythonQt.h include/PythonQt5/PythonQt.h include/Qt5Python27/PythonQt/PythonQt.h DOC "Directory where PythonQt was installed.")
find_path(PYTHONQT_PYTHON2_INCLUDE_DIR PythonQt.h PATHS "${PYTHONQT_PYTHON2_INSTALL_DIR}/include/PythonQt" "${PYTHONQT_PYTHON2_INSTALL_DIR}/include/PythonQt5" "${PYTHONQT_PYTHON2_INSTALL_DIR}/include/Qt5Python27/PythonQt" DOC "Path to the PythonQt include directory")
find_library(PYTHONQT_PYTHON2_LIBRARY NAMES PythonQt QtPython PythonQt-Qt5-Python2.7 Qt5Python27 PATHS "${PYTHONQT_PYTHON2_INSTALL_DIR}/lib" DOC "The PythonQt library.")

mark_as_advanced(PYTHONQT_PYTHON2_INSTALL_DIR)
mark_as_advanced(PYTHONQT_PYTHON2_INCLUDE_DIR)
mark_as_advanced(PYTHONQT_PYTHON2_LIBRARY)

# Python3
find_path(PYTHONQT_INSTALL_DIR NAMES include/PythonQt/PythonQt.h include/PythonQt5/PythonQt.h include/Qt5Python35/PythonQt/PythonQt.h DOC "Directory where PythonQt was installed.")
find_path(PYTHONQT_INCLUDE_DIR PythonQt.h PATHS "${PYTHONQT_INSTALL_DIR}/include/PythonQt" "${PYTHONQT_INSTALL_DIR}/include/PythonQt5" "${PYTHONQT_INSTALL_DIR}/include/Qt5Python35/PythonQt" DOC "Path to the PythonQt include directory")
find_library(PYTHONQT_LIBRARY NAMES PythonQt QtPython PythonQt-Qt5-Python3.6 PATHS "${PYTHONQT_INSTALL_DIR}/lib" DOC "The PythonQt library.")

mark_as_advanced(PYTHONQT_INSTALL_DIR)
mark_as_advanced(PYTHONQT_INCLUDE_DIR)
mark_as_advanced(PYTHONQT_LIBRARY)

# On linux, also find libutil
if(UNIX AND NOT APPLE)
  find_library(PYTHONQT_LIBUTIL util)
  mark_as_advanced(PYTHONQT_LIBUTIL)
endif()

set(PYTHONQT_FOUND 0)
if(PYTHONQT_INCLUDE_DIR AND PYTHONQT_LIBRARY)
  # Currently CMake'ified PythonQt only supports building against a python Release build.
  # This applies independently of CTK build type (Release, Debug, ...)
  add_definitions(-DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK)
  set(PYTHONQT_FOUND 1)
  set(PYTHONQT_INCLUDE_DIRS ${PYTHONQT_INCLUDE_DIR})
  set(PYTHONQT_LIBRARIES ${PYTHONQT_LIBRARY} ${PYTHONQT_LIBUTIL})
  set(PYTHONQT_PYTHON2 OFF)
elseif(PYTHONQT_PYTHON2_INCLUDE_DIR AND PYTHONQT_PYTHON2_LIBRARY)
  message(STATUS "Using Python2 version of PythonQt.")
  # Currently CMake'ified PythonQt only supports building against a python Release build.
  # This applies independently of CTK build type (Release, Debug, ...)
  add_definitions(-DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK)
  set(PYTHONQT_FOUND 1)
  set(PYTHONQT_INCLUDE_DIRS ${PYTHONQT_PYTHON2_INCLUDE_DIR})
  set(PYTHONQT_LIBRARIES ${PYTHONQT_PYTHON2_LIBRARY} ${PYTHONQT_PYTHON2_LIBUTIL})
  set(PYTHONQT_PYTHON2 ON)
endif()

