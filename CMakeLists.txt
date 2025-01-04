cmake_minimum_required(VERSION 3.5.0)
project(OpenGLTutorial VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 23)

# Set the policy to use GLVND if available
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

find_package(GLFW3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/external/glad/include)
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)
include_directories(${PROJECT_SOURCE_DIR}/external)
include_directories(${PROJECT_SOURCE_DIR}/external/glm)

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    message("clang-format found.")
    # Define the target that formats all C++ source files
    add_custom_target(
        clang_format ALL
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.hpp
        # Include all subdirectories you want to format
        # COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/shaders/*.fs ${PROJECT_SOURCE_DIR}/shaders/*.vs
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running clang-format on source files"
    )
endif()

add_executable(OpenGLTutorial main.cpp Shader.cpp Camera.cpp)
target_link_libraries(OpenGLTutorial OpenGL::GL glfw glad -lGL)

# Copy shader files to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders)
file(COPY ${CMAKE_SOURCE_DIR}/textures/ DESTINATION ${CMAKE_BINARY_DIR}/textures)

target_include_directories(OpenGLTutorial PRIVATE ${PROJECT_SOURCE_DIR}/external/glad/include ${OPENGL_INCLUDE_DIR})
include(CTest)
enable_testing()

