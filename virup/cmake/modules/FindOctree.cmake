# - Try to find liboctree
# Once done this will define
#  OCTREE_FOUND - System has liboctree
#  OCTREE_INCLUDE_DIRS - The liboctree include directories
#  OCTREE_LIBRARIES - The libraries needed to use liboctree
#  OCTREE_DEFINITIONS - Compiler switches required for using liboctree

find_package(PkgConfig)
pkg_check_modules(PC_OCTREE QUIET liboctree)
set(OCTREE_DEFINITIONS ${PC_OCTREE_CFLAGS_OTHER})

find_path(OCTREE_INCLUDE_DIR liboctree/Octree.hpp
          HINTS ${PC_OCTREE_INCLUDEDIR} ${PC_OCTREE_INCLUDE_DIRS}
          PATH_SUFFIXES liboctree )

find_library(OCTREE_LIBRARY NAMES octree
             HINTS ${PC_OCTREE_LIBDIR} ${PC_OCTREE_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OCTREE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Octree  DEFAULT_MSG
                                  OCTREE_LIBRARY OCTREE_INCLUDE_DIR)

mark_as_advanced(OCTREE_INCLUDE_DIR OCTREE_LIBRARY )

set(OCTREE_LIBRARIES ${OCTREE_LIBRARY} )
set(OCTREE_INCLUDE_DIRS ${OCTREE_INCLUDE_DIR} )
