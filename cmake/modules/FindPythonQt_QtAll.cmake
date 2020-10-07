# Find PythonQt_QtAll
#
# Sets PYTHONQT_QTALL_FOUND, PYTHONQT_QTALL_INCLUDE_DIRS, PYTHONQT_QTALL_LIBRARY, PYTHONQT_QTALL_LIBRARIES
#

#Python2
find_path(PYTHONQT_QTALL_PYTHON2_INSTALL_DIR NAMES include/PythonQt/PythonQt_QtAll.h include/Qt5Python27/PythonQt/PythonQt_QtAll.h include/PythonQt5/PythonQt_QtAll.h DOC "Directory where PythonQt_QtAll.h was installed.")
find_path(PYTHONQT_QTALL_PYTHON2_INCLUDE_DIR PythonQt_QtAll.h PATHS "${PYTHONQT_QTALL_PYTHON2_INSTALL_DIR}/include/PythonQt" "${PYTHONQT_QTALL_PYTHON2_INSTALL_DIR}/include/PythonQt5" "${PYTHONQT_QTALL_PYTHON2_INSTALL_DIR}/include/Qt5Python27/PythonQt" DOC "Path to the PythonQt_QtAll include directory")
find_library(PYTHONQT_QTALL_PYTHON2_LIBRARY NAMES PythonQt_QtAll QtPython_QtAll PythonQt_QtAll-Qt5-Python3.6 PATHS "${PYTHONQT_QTALL_PYTHON2_INSTALL_DIR}/extensions/PythonQt_QtAll" DOC "The PythonQt_QtAll library.")

mark_as_advanced(PYTHONQT_QTALL_PYTHON2_INSTALL_DIR)
mark_as_advanced(PYTHONQT_QTALL_PYTHON2_INCLUDE_DIR)
mark_as_advanced(PYTHONQT_QTALL_PYTHON2_LIBRARY)

#Python3
find_path(PYTHONQT_QTALL_INSTALL_DIR NAMES include/PythonQt/PythonQt_QtAll.h include/Qt5Python35/PythonQt/PythonQt_QtAll.h include/PythonQt5/PythonQt_QtAll.h DOC "Directory where PythonQt_QtAll.h was installed.")
find_path(PYTHONQT_QTALL_INCLUDE_DIR PythonQt_QtAll.h PATHS "${PYTHONQT_QTALL_INSTALL_DIR}/include/PythonQt" "${PYTHONQT_QTALL_INSTALL_DIR}/include/PythonQt5" "${PYTHONQT_QTALL_INSTALL_DIR}/include/Qt5Python35/PythonQt" DOC "Path to the PythonQt_QtAll include directory")
find_library(PYTHONQT_QTALL_LIBRARY NAMES PythonQt_QtAll QtPython_QtAll PythonQt_QtAll-Qt5-Python3.6 PATHS "${PYTHONQT_QTALL_INSTALL_DIR}/extensions/PythonQt_QtAll" DOC "The PythonQt_QtAll library.")

mark_as_advanced(PYTHONQT_QTALL_INSTALL_DIR)
mark_as_advanced(PYTHONQT_QTALL_INCLUDE_DIR)
mark_as_advanced(PYTHONQT_QTALL_LIBRARY)

set(PYTHONQT_QTALL_FOUND 0)
if(PYTHONQT_QTALL_INCLUDE_DIR AND PYTHONQT_QTALL_LIBRARY)
  # Currently CMake'ified PYTHONQT_QTALL only supports building against a python Release build.
  # This applies independently of CTK build type (Release, Debug, ...)
  add_definitions(-DPYTHONQT_QTALL_USE_RELEASE_PYTHON_FALLBACK)
  set(PYTHONQT_QTALL_FOUND 1)
  set(PYTHONQT_QTALL_INCLUDE_DIRS ${PYTHONQT_QTALL_INCLUDE_DIR})
  set(PYTHONQT_QTALL_LIBRARIES ${PYTHONQT_QTALL_LIBRARY} ${PYTHONQT_QTALL_LIBUTIL})
elseif(PYTHONQT_QTALL_PYTHON2_INCLUDE_DIR AND PYTHONQT_QTALL_PYTHON2_LIBRARY)
  # Currently CMake'ified PYTHONQT_QTALL_PYTHON2 only supports building against a python Release build.
  # This applies independently of CTK build type (Release, Debug, ...)
  add_definitions(-DPYTHONQT_QTALL_USE_RELEASE_PYTHON_FALLBACK)
  set(PYTHONQT_QTALL_FOUND 1)
  set(PYTHONQT_QTALL_INCLUDE_DIRS ${PYTHONQT_QTALL_PYTHON2_INCLUDE_DIR})
  set(PYTHONQT_QTALL_LIBRARIES ${PYTHONQT_QTALL_PYTHON2_LIBRARY} ${PYTHONQT_QTALL_PYTHON2_LIBUTIL})
endif()

