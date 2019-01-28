# Find the LeapMotion SDK
# NOTE: there is no default installation path as the code needs to be build
# This module defines:
# LEAPMOTION_FOUND, if false do not try to link against the LeapMotion SDK
# LEAPMOTION_LIBRARY, the name of the LeapMotion SDK library to link against
# LEAPMOTION_INCLUDE_DIR, the LeapMotion VR SDK include directory
#
# You can also specify the environment variable LEAPMOTION_DIR or define it with
# -DLEAPMOTION_DIR=... to hint at the module where to search for the LeapMotion SDK if it's
# installed in a non-standard location.

find_path(LEAPMOTION_INCLUDE_DIR Leap.h
	HINTS
	${LEAPMOTION_DIR}
	$ENV{LEAPMOTION_DIR}
	PATH_SUFFIXES headers/
	# TODO: Unsure on handling of the possible default install locations
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/include/
	/usr/include/
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	if (UNIX OR MINGW)
		set(LIB_PATH_SUFFIX "lib/linux64/")
	elseif (MSVC)
		set(LIB_PATH_SUFFIX "lib/win64/")
	else()
		message(ERROR "Error: Unsupported 64 bit configuration")
	endif()
else()
	if (UNIX OR MINGW)
		set(LIB_PATH_SUFFIX "lib/linux32/")
	elseif (MSVC)
		set(LIB_PATH_SUFFIX "lib/win32/")
	elseif(APPLE)
		set(LIB_PATH_SUFFIX "lib/osx32/")
	else()
		message(ERROR "Error: Unsupported 32 bit configuration")
	endif()
endif()

find_library(LEAPMOTION_LIBRARY_TMP NAMES Leap Leap.lib
	HINTS
	${LEAPMOTION_DIR}
	$ENV{LEAPMOTION_DIR}
	PATH_SUFFIXES ${LIB_PATH_SUFFIX}
	# TODO: I don't know if these will be correct if people have installed
	# the library on to their system instead of just using the git repo or w/e
	PATHS
	/sw
	/opt/local
	/opt/csw
	/opt
)

set(LEAPMOTION_FOUND FALSE)
if (LEAPMOTION_LIBRARY_TMP AND LEAPMOTION_INCLUDE_DIR)
	set(LEAPMOTION_LIBRARY ${LEAPMOTION_LIBRARY_TMP} CACHE STRING "Which Leap Motion library to link against")
	set(LEAPMOTION_LIBRARY_TMP ${LEAPMOTION_LIBRARY_TMP} CACHE INTERNAL "")
	set(LEAPMOTION_FOUND TRUE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LeapMotion REQUIRED_VARS LEAPMOTION_LIBRARY LEAPMOTION_INCLUDE_DIR)

