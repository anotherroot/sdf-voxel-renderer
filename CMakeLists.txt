cmake_minimum_required(VERSION 3.17)
project(
  map_maker
  VERSION "0.1.0"
  DESCRIPTION "Map Maker for Voxel City"
  LANGUAGES CXX C
  )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g")
endif()

add_subdirectory(ext)
add_subdirectory(src)
