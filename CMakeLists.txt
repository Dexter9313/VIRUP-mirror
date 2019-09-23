#Copyright(C) 2018 Florian Cabot < florian.cabot @hotmail.fr >
#
#This program is free software; you can redistribute it and / or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.

#INIT
cmake_minimum_required(VERSION 3.0.0)

#READ CONFIG FILE AND SET VARIABLES
set(BUILD_CONF "${CMAKE_CURRENT_SOURCE_DIR}/build.conf")
if(NOT EXISTS "${BUILD_CONF}")
	set(BUILD_CONF "${CMAKE_CURRENT_SOURCE_DIR}/build.conf.example")
endif()
message(STATUS "Build configuration file: ${BUILD_CONF}")
file(STRINGS "${BUILD_CONF}" ConfigContents)
foreach(NameAndValue ${ConfigContents})
#Strip leading spaces
  string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
#Find variable name
  string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
#Find the value
  string(REPLACE "${Name}=" "" Value ${NameAndValue})
#Remove quotes
  string(REPLACE "\"" "" Value ${Value})
#Set the variable
  set(${Name} ${Value})
endforeach()

project(${PROJECT_NAME} CXX)
set(CMAKE_PROJECT_DESCRIPTION ${PROJECT_DESCRIPTION})

#C++ 11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#For clang - tidy to use(clang - tidy - p)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#WARNINGS
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wsuggest-override")
	if(DEFINED WERROR)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	endif()
endif()
set(unusedVal "${WERROR}")

#Qt options
if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

#Define project name in source for preprocessor
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DPROJECT_DIRECTORY="${PROJECT_DIRECTORY}")
add_definitions(-DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

message(STATUS "Project name: ${PROJECT_NAME}")
message(STATUS "Project dir: ${PROJECT_DIRECTORY}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

#FILES / DIRECTORIES

# Update submodules

execute_process(COMMAND git submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(EXISTS ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty)
	file(GLOB THIRDPARTY_INCLUDES_FILES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/**/include/* LIST_DIRECTORIES true)
	list(GET THIRDPARTY_INCLUDES_FILES 0 THIRDPARTY_INCLUDES_FILE)
	get_filename_component(RESULT ${THIRDPARTY_INCLUDES_FILE} DIRECTORY)

	file(GLOB THIRDPARTY_DATA_FILES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/**/data LIST_DIRECTORIES true)
endif()

set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
set(THIRDPARTY_INCLUDES ${RESULT})
set(PROJECT_INCLUDES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/include)
set(TEST_INCLUDES ${PROJECT_SOURCE_DIR}/test ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/test)

file(GLOB_RECURSE ENGINE_HPP_FILES ${COMMON_INCLUDES}/*.hpp)
file(GLOB_RECURSE THIRDPARTY_HPP_FILES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/**/include/*.hpp)
file(GLOB_RECURSE PROJECT_HPP_FILES ${PROJECT_INCLUDES}/*.hpp)
file(GLOB_RECURSE HPP_FILES ${ENGINE_HPP_FILES} ${THIRDPARTY_HPP_FILES} ${PROJECT_HPP_FILES})

file(GLOB_RECURSE ENGINE_SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE THIRDPARTY_SRC_FILES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/**/src/*.cpp)
file(GLOB_RECURSE PROJECT_SRC_FILES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/src/*.cpp)
file(GLOB_RECURSE SRC_FILES ${ENGINE_SRC_FILES} ${THIRDPARTY_SRC_FILES} ${PROJECT_SRC_FILES})
file(GLOB_RECURSE MAIN_FILE ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM SRC_FILES ${MAIN_FILE})

file(GLOB_RECURSE ENGINE_TEST_HPP_FILES ${PROJECT_SOURCE_DIR}/test/*.hpp)
file(GLOB_RECURSE TEST_HPP_FILES ${ENGINE_TEST_HPP_FILES} ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/test/*.hpp)
file(GLOB_RECURSE ENGINE_TEST_CPP_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
file(GLOB_RECURSE TEST_SRC_FILES ${ENGINE_TEST_CPP_FILES} ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/test/*.cpp)

FOREACH(PATH ${THIRDPARTY_DATA_FILES})
	get_filename_component(FOO "${PATH}" DIRECTORY)
	get_filename_component(LIB_NAME "${FOO}" NAME)
	set(LIBRARIES_DIRS "${LIBRARIES_DIRS} ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/${LIB_NAME}/include ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/${LIB_NAME}/src")
ENDFOREACH(PATH)

if(DOC_ONLY)
	include(cmake/Doc.cmake)
	return()
endif()

#DEPENDENCIES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/modules)

#Qt
find_package(Qt5 COMPONENTS Widgets Test REQUIRED)

#Assimp
set(ASSIMP_INCLUDE_DIRS $ENV{ASSIMP_INCLUDE_DIRS})
set(ASSIMP_LIBRARIES $ENV{ASSIMP_LIBRARIES})
if((NOT DEFINED ASSIMP_INCLUDE_DIRS) OR (NOT DEFINED ASSIMP_LIBRARIES))
	find_package(assimp REQUIRED)
endif()

#OpenVR
set(OPENVR_INCLUDE_DIR $ENV{OPENVR_INCLUDE_DIR})
set(OPENVR_LIBRARY $ENV{OPENVR_LIBRARY})
if((NOT DEFINED OPENVR_INCLUDE_DIR) OR (NOT DEFINED OPENVR_LIBRARY))
	find_package(OpenVR REQUIRED)
endif()

#LeapMotionSDK
set(LEAPMOTION_INCLUDE_DIR $ENV{LEAPMOTION_INCLUDE_DIR})
set(LEAPMOTION_LIBRARY $ENV{LEAPMOTION_LIBRARY})
if((NOT DEFINED LEAPMOTION_INCLUDE_DIR) OR (NOT DEFINED LEAPMOTION_LIBRARY))
	find_package(LeapMotion)
else()
	set(LEAPMOTION_FOUND true)
endif()

#PythonQt
set(PYTHONQT_INCLUDE_DIRS $ENV{PYTHONQT_INCLUDE_DIRS})
set(PYTHONQT_LIBRARIES $ENV{PYTHONQT_LIBRARIES})
set(PYTHONQT_QTALL_INCLUDE_DIRS $ENV{PYTHONQT_QTALL_INCLUDE_DIRS})
set(PYTHONQT_QTALL_LIBRARIES $ENV{PYTHONQT_QTALL_LIBRARIES})
if((NOT DEFINED PYTHONQT_INCLUDE_DIRS) OR (NOT DEFINED PYTHONQT_LIBRARIES))
	find_package(PythonQt)
else()
	set(PYTHONQT_FOUND true)
endif()
if((NOT DEFINED PYTHONQT_QTALL_INCLUDE_DIRS) OR (NOT DEFINED PYTHONQT_QTALL_LIBRARIES))
	find_package(PythonQt_QtAll REQUIRED)
else()
	set(PYTHONQT_QTALL_FOUND true)
endif()

if(PYTHONQT_FOUND)
	message(STATUS "Found PythonQt : ${PYTHONQT_INCLUDE_DIRS} | ${PYTHONQT_LIBRARIES}")
else()
	message(STATUS "PythonQt not found.")
endif()
if(PYTHONQT_QTALL_FOUND)
	message(STATUS "Found PythonQt_All : ${PYTHONQT_QTALL_INCLUDE_DIRS} | ${PYTHONQT_QTALL_LIBRARIES}")
else()
	message(STATUS "PythonQt_All not found.")
endif()

#Python
if((PYTHONQT_FOUND) OR (PYTHONQT_QTALL_FOUND))
	find_package(PythonLibs)
endif()

#Additional
include(${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/cmake/Dependencies.cmake)

#INCLUDE

set(EXTERNAL_LIBS_INCLUDE_DIRS ${Qt5_DIR} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${OPENVR_INCLUDE_DIR} ${PROJECT_INCLUDE_DIRS})
if(LEAPMOTION_FOUND)
	add_definitions(-DLEAP_MOTION)
	set(EXTERNAL_LIBS_INCLUDE_DIRS ${EXTERNAL_LIBS_INCLUDE_DIRS} ${LEAPMOTION_INCLUDE_DIR})
endif()
if(PYTHONQT_FOUND)
	add_definitions(-DPYTHONQT)
	set(EXTERNAL_LIBS_INCLUDE_DIRS ${EXTERNAL_LIBS_INCLUDE_DIRS} ${PYTHONQT_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
endif()
if(PYTHONQT_QTALL_FOUND)
	add_definitions(-DPYTHONQT_QTALL)
	set(EXTERNAL_LIBS_INCLUDE_DIRS ${EXTERNAL_LIBS_INCLUDE_DIRS} ${PYTHONQT_QTALL_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
endif()

include_directories(${COMMON_INCLUDES} ${THIRDPARTY_INCLUDES} ${PROJECT_INCLUDES} ${TEST_INCLUDES} SYSTEM ${EXTERNAL_LIBS_INCLUDE_DIRS})

#LINKING SETUP
set(LD_LIBS ${CMAKE_DL_LIBS} Qt5::Widgets Qt5::Test ${ASSIMP_LIBRARIES} ${OPENVR_LIBRARY} ${PROJECT_LIBRARIES})
if(LEAPMOTION_FOUND)
	set(LD_LIBS ${LD_LIBS} ${LEAPMOTION_LIBRARY})
endif()
if(PYTHONQT_FOUND)
	set(LD_LIBS ${LD_LIBS} ${PYTHONQT_LIBRARIES} ${PYTHON_LIBRARIES})
endif()
if(PYTHONQT_QTALL_FOUND)
	set(LD_LIBS ${LD_LIBS} ${PYTHONQT_QTALL_LIBRARIES} ${PYTHON_LIBRARIES})
endif()

#Find .so in working directory
if(NOT WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")
endif()

#prevent console from popping on Windows
IF( WIN32 )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /entry:mainCRTStartup")
ENDIF()


#RUN MOC (FOR QT5)
qt5_wrap_cpp(MOC_FILES ${HPP_FILES})
qt5_wrap_cpp(TEST_MOC_FILES ${TEST_HPP_FILES})

#COMMON OBJECTS
add_library(objects OBJECT ${SRC_FILES} ${MOC_FILES})

#TESTS
add_executable(tests ${TEST_SRC_FILES} ${TEST_MOC_FILES} $<TARGET_OBJECTS:objects>)
target_link_libraries(tests ${LD_LIBS})

#EXECUTABLE : PROJECT_NAME
add_executable(${PROJECT_NAME} ${MAIN_FILE} $<TARGET_OBJECTS:objects>)
target_link_libraries(${PROJECT_NAME} ${LD_LIBS})


#SETUP DATA DIRECTORY

#core
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/data ${PROJECT_BINARY_DIR}/data
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#project
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/data ${PROJECT_BINARY_DIR}/data/${PROJECT_DIRECTORY}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#thirdparty

FOREACH(PATH ${THIRDPARTY_DATA_FILES})
	get_filename_component(FOO "${PATH}" DIRECTORY)
	get_filename_component(LIB_NAME "${FOO}" NAME)
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/thirdparty/${LIB_NAME}/data ${PROJECT_BINARY_DIR}/data/${LIB_NAME}
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
ENDFOREACH(PATH)

#translations

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/translations ${PROJECT_BINARY_DIR}/data/translations
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


#INSTALL RULES / NOT FOR WINDOWS
if(NOT WIN32)
	INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION bin)
	INSTALL(FILES ${OPENVR_LIBRARY} DESTINATION lib)
	if(LEAPMOTION_FOUND)
		INSTALL(FILES ${LEAPMOTION_LIBRARY} DESTINATION lib)
	endif()
	if(PYTHONQT_FOUND)
		INSTALL(FILES ${PYTHONQT_LIBRARIES} DESTINATION lib)
	endif()
	if(PYTHONQT_QTALL_FOUND)
		INSTALL(FILES ${PYTHONQT_QTALL_LIBRARIES} DESTINATION lib)
	endif()
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data DESTINATION share/${PROJECT_NAME})
	include(${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/cmake/Install.cmake)

	SET(CPACK_GENERATOR "DEB")
	SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER ${PROJECT_MAINTAINER})
	if(TAG)
		SET(CPACK_PACKAGE_VERSION ${TAG})
	endif()

	SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5:amd64,\
	libpythonqt-qt5-python3-3:amd64,\
	libpythonqt-qtall-qt5-python3-3:amd64,\
	libqt5gui5:amd64,\
	libqt5core5a:amd64,\
	libstdc++6:amd64,\
	libc6:amd64,\
	libgcc1:amd64,\
	libpython3.6:amd64,\
	libqt5svg5:amd64,\
	libqt5opengl5:amd64,\
	libqt5printsupport5:amd64,\
	libqt5multimediawidgets5:amd64,\
	libqt5quickwidgets5:amd64,\
	libqt5multimedia5:amd64,\
	libqt5quick5:amd64,\
	libqt5sql5:amd64,\
	libqt5xmlpatterns5,\
	libqt5qml5:amd64,\
	libqt5network5:amd64,\
	libqt5xml5:amd64,\
	libgl1:amd64,\
	libpng16-16:amd64,\
	libharfbuzz0b:amd64,\
	zlib1g:amd64,\
	libicu60:amd64,\
	libdouble-conversion1:amd64,\
	libglib2.0-0:amd64,\
	libexpat1:amd64,\
	libpulse0:amd64,\
	libglx0:amd64,\
	libglvnd0:amd64,\
	libfreetype6:amd64,\
	libgraphite2-3:amd64,\
	libicu60:amd64,\
	libpcre3:amd64,\
	libpulse0:amd64,\
	libdbus-1-3:amd64,\
	libx11-6:amd64,\
	libxcb1:amd64,\
	libsystemd0:amd64,\
	libwrap0:amd64,\
	libsndfile1:amd64,\
	libasyncns0:amd64,\
	libxau6:amd64,\
	libxdmcp6:amd64,\
	liblzma5:amd64,\
	liblz4-1:amd64,\
	libgcrypt20:amd64,\
	libflac8:amd64,\
	libogg0:amd64,\
	libvorbis0a:amd64,\
	libvorbisenc2:amd64,\
	libgpg-error0:amd64,\
	libbsd0:amd64")

	if(NOT "${PROJECT_ADDITIONAL_DEB_DEPENDENCIES}" STREQUAL "")
		message(STATUS "${PROJECT_ADDITIONAL_DEB_DEPENDENCIES}")
		set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS},${PROJECT_ADDITIONAL_DEB_DEPENDENCIES}")
		message(STATUS "${CPACK_DEBIAN_PACKAGE_DEPENDS}")
	endif()

	INCLUDE(CPack)

	#Create uninstall target
	add_custom_target(uninstall
			COMMAND xargs rm < install_manifest.txt && rm install_manifest.txt
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Uninstalling executable..."
			VERBATIM)
endif()

# Including extra cmake rules
include(cmake/ClangDevTools.cmake)
include(cmake/Doc.cmake)


# Translation
if(PROJECT_TRANSLATE)
	set(TRANSLATION_FILES ${TRANSLATION_FILES} ${PROJECT_HPP_FILES} ${PROJECT_SRC_FILES})
endif()
if(PROJECT_TRANSLATE_ENGINE)
	set(TRANSLATION_FILES ${TRANSLATION_FILES} ${ENGINE_HPP_FILES} ${ENGINE_SRC_FILES})
endif()
if(PROJECT_TRANSLATE_THIRDPARTY)
	set(TRANSLATION_FILES ${TRANSLATION_FILES} ${THIRDPARTY_HPP_FILES} ${THIRDPARTY_SRC_FILES})
endif()

add_custom_target(translation-update
	COMMAND lupdate ${TRANSLATION_FILES} -ts ${CMAKE_CURRENT_SOURCE_DIR}/translations/${PROJECT_NAME}_fr.ts
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating translation (.ts) file."
	VERBATIM)
add_custom_target(translation-release
	COMMAND lrelease ${CMAKE_CURRENT_SOURCE_DIR}/translations/${PROJECT_NAME}_fr.ts
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating translation (.qm) file."
	VERBATIM)
