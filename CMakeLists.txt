cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(Octree VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

find_package(VTK COMPONENTS 
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "Octree: Unable to find the VTK build folder.")
endif()

add_executable(Octree main.cpp src/classes.cpp)
target_link_libraries(Octree PRIVATE ${VTK_LIBRARIES})
target_include_directories(Octree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

vtk_module_autoinit(
  TARGETS Octree
  MODULES ${VTK_LIBRARIES}
)