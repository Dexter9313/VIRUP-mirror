set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/virup/cmake/modules)

set(OCTREE_INCLUDE_DIR $ENV{OCTREE_INCLUDE_DIR})
set(OCTREE_LIBRARY $ENV{OCTREE_LIBRARY})

if((NOT DEFINED OCTREE_INCLUDE_DIR) OR (NOT DEFINED OCTREE_LIBRARY))
       find_package(Octree REQUIRED)
endif()

find_package (Threads)

find_package(Boost REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)

set(PROJECT_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS})
set(PROJECT_LIBRARIES ${Boost_LIBRARIES} -lquadmath Qt5::Concurrent)

set(PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${OCTREE_INCLUDE_DIR})
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${OCTREE_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
