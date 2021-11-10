cmake_minimum_required(VERSION 3.17)


add_library(renderer STATIC)


target_sources(renderer
  PRIVATE
  src/shader.cpp
  src/texture.cpp
  src/camera.cpp
  src/render_pass.cpp
  src/renderer.cpp
  # src/octree.cpp
  )
target_link_libraries(renderer PUBLIC glad glm imgui)
target_include_directories(renderer PUBLIC include )
