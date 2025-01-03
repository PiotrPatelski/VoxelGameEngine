cmake_minimum_required(VERSION 3.5.0)
project(OpenGLTutorial VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 23)

# Set the policy to use GLVND if available
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

find_package(GLFW3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/glad/include)
add_library(glad STATIC glad/src/glad.c)

add_executable(OpenGLTutorial main.cpp Shader.hpp Shader.cpp)
target_link_libraries(OpenGLTutorial OpenGL::GL glfw glad -lGL)

# Copy shader files to the build directory
configure_file(${CMAKE_SOURCE_DIR}/shaders/vertex_shader.vs ${CMAKE_BINARY_DIR}/shaders/vertex_shader.vs COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/fragment_shader1.fs ${CMAKE_BINARY_DIR}/shaders/fragment_shader1.fs COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/fragment_shader2.fs ${CMAKE_BINARY_DIR}/shaders/fragment_shader2.fs COPYONLY)

target_include_directories(OpenGLTutorial PRIVATE glad/include ${OPENGL_INCLUDE_DIR})
include(CTest)
enable_testing()

