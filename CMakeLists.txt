#    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#INIT
cmake_minimum_required(VERSION 3.0.0)

#READ CONFIG FILE AND SET VARIABLES
file(STRINGS "build.conf" ConfigContents)
foreach(NameAndValue ${ConfigContents})
  # Strip leading spaces
  string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
  # Find variable name
  string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
  # Find the value
  string(REPLACE "${Name}=" "" Value ${NameAndValue})
  # Remove quotes
  string(REPLACE "\"" "" Value ${Value})
  # Set the variable
  set(${Name} ${Value})
endforeach()

project(${PROJECT_NAME} CXX)
set(CMAKE_PROJECT_DESCRIPTION ${PROJECT_DESCRIPTION})


#C++11
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

#WARNINGS
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wsuggest-override")
	if(DEFINED WERROR)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	endif()
endif()

#Qt options
if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

#Define project name in source for preprocessor
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")


#FILES/DIRECTORIES

set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_INCLUDES ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/include)

file(GLOB_RECURSE HPP_FILES ${COMMON_INCLUDES}/*.hpp ${PROJECT_INCLUDES}/*.hpp)
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/src/*.cpp)

#DEPENDENCIES
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/modules)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
if((NOT DEFINED OPENVR_INCLUDE_DIR) OR (NOT DEFINED OPENVR_LIBRARY))
	find_package(OpenVR REQUIRED)
endif()
if((NOT DEFINED LEAPMOTION_INCLUDE_DIR) OR (NOT DEFINED LEAPMOTION_LIBRARY))
	find_package(LeapMotion)
else()
	set(LEAPMOTION_FOUND true)
endif()

include(${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/cmake/Dependencies.cmake)

#INCLUDE

set(EXTERNAL_LIBS_INCLUDE_DIRS ${Qt5_DIR} ${Qt5Widgets_INCLUDE_DIRS} ${OPENVR_INCLUDE_DIR} ${PROJECT_INCLUDE_DIRS})
if(LEAPMOTION_FOUND)
	add_definitions(-DLEAP_MOTION)
	set(EXTERNAL_LIBS_INCLUDE_DIRS ${EXTERNAL_LIBS_INCLUDE_DIRS} ${LEAPMOTION_INCLUDE_DIR})
endif()

include_directories(${COMMON_INCLUDES} ${PROJECT_INCLUDES} SYSTEM ${EXTERNAL_LIBS_INCLUDE_DIRS})

#LINKING SETUP
	set(LD_LIBS Qt5::Widgets ${OPENVR_LIBRARY} ${PROJECT_LIBRARIES})
if(LEAPMOTION_FOUND)
	set(LD_LIBS Qt5::Widgets ${LD_LIBS} ${LEAPMOTION_LIBRARY})
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

#EXECUTABLE : PROJECT_NAME
add_executable(${PROJECT_NAME} ${SRC_FILES} ${MOC_FILES})
target_link_libraries(${PROJECT_NAME} ${LD_LIBS})


#INSTALL RULES
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION bin)
INSTALL(FILES ${OPENVR_LIBRARY} DESTINATION lib)
if(LEAPMOTION_FOUND)
	INSTALL(FILES ${LEAPMOTION_LIBRARY} DESTINATION lib)
endif()
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION share/${PROJECT_NAME})
include(${PROJECT_SOURCE_DIR}/${PROJECT_DIRECTORY}/cmake/Install.cmake)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER ${PROJECT_MAINTAINER})
if(TAG)
	SET(CPACK_PACKAGE_VERSION ${TAG})
endif()

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5:amd64,\
libqt5gui5:amd64,\
libqt5core5a:amd64,\
libstdc++6:amd64,\
libc6:amd64,\
libgcc1:amd64,\
libc6:amd64,\
libc6:amd64,\
libc6:amd64,\
libc6:amd64,\
libgl1:amd64,\
libpng16-16:amd64,\
libharfbuzz0b:amd64,\
zlib1g:amd64,\
libicu60:amd64,\
libicu60:amd64,\
libdouble-conversion1:amd64,\
libglib2.0-0:amd64,\
libglx0:amd64,\
libglvnd0:amd64,\
libfreetype6:amd64,\
libgraphite2-3:amd64,\
libicu60:amd64,\
libpcre3:amd64,\
libx11-6:amd64,\
libxcb1:amd64,\
libxau6:amd64,\
libxdmcp6:amd64,\
libbsd0:amd64,\
${PROJECT_ADDITIONAL_DEB_DEPENDENCIES}")

INCLUDE(CPack)

#Create uninstall target
add_custom_target(uninstall
		COMMAND xargs rm < install_manifest.txt && rm install_manifest.txt
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Uninstalling executable..."
		VERBATIM)


