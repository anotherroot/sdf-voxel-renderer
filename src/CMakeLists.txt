cmake_minimum_required(VERSION 3.17)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${map_maker_SOURCE_DIR}/build/Debug)
else()
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${map_maker_SOURCE_DIR}/build/Release)
endif()

add_executable(desktop)
target_sources(
  desktop
  PRIVATE
  
  main.cpp
  camera_controller.cpp
  renderer/render_pass.cpp
  renderer/shader.cpp
  renderer/compute_pass.cpp
  renderer/texture.cpp
  renderer/renderer.cpp
  renderer/camera.cpp
  voxel.cpp
  window.cpp 

  )

target_link_libraries(desktop PUBLIC  glm json ogt_vox  glad glfw imgui)
target_include_directories(desktop PUBLIC renderer )
