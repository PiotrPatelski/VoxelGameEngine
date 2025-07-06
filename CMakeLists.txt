cmake_minimum_required(VERSION 3.5.0)
project(PioterCraft VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

# Set the policy to use GLVND if available
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

find_package(GLFW3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/external/glad/include)
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)
include_directories(${PROJECT_SOURCE_DIR}/external)
include_directories(${PROJECT_SOURCE_DIR}/external/glm)
include_directories(${ASSIMP_INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(ASSIMP REQUIRED assimp)

# Cross-platform FreeType detection
if(WIN32)
    # For Windows, you may have to manually specify where FreeType is located if it's not installed globally
    find_path(FREETYPE_INCLUDE_DIRS freetype/ft2build.h)
    find_library(FREETYPE_LIBRARIES NAMES freetype)
else()
    # For Linux, FreeType is usually available via package managers
    find_package(Freetype REQUIRED)
endif()

# Check if FreeType was found
if(NOT FREETYPE_INCLUDE_DIRS OR NOT FREETYPE_LIBRARIES)
    message(FATAL_ERROR "FreeType was not found or not correctly configured.")
endif()

include_directories(${FREETYPE_INCLUDE_DIRS})

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    message("clang-format found.")
    # Define the target that formats all C++ source files
    add_custom_target(
        clang_format ALL
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/main.cpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/core/src/*.cpp ${PROJECT_SOURCE_DIR}/core/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/world/src/*.cpp ${PROJECT_SOURCE_DIR}/world/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/entities/src/*.cpp ${PROJECT_SOURCE_DIR}/entities/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/physics/src/*.cpp ${PROJECT_SOURCE_DIR}/physics/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/rendering/src/*.cpp ${PROJECT_SOURCE_DIR}/rendering/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/ui/src/*.cpp ${PROJECT_SOURCE_DIR}/ui/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/water/src/*.cpp ${PROJECT_SOURCE_DIR}/water/inc/*.hpp
        COMMAND ${CLANG_FORMAT} -i ${PROJECT_SOURCE_DIR}/utils/src/*.cpp ${PROJECT_SOURCE_DIR}/utils/inc/*.hpp
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running clang-format on source files"
    )
endif()

# Create the main executable
add_executable(PioterCraft main.cpp)

# Add all module include directories to the global include path
target_include_directories(PioterCraft PRIVATE 
    ${PROJECT_SOURCE_DIR}/core/inc
    ${PROJECT_SOURCE_DIR}/world/inc
    ${PROJECT_SOURCE_DIR}/entities/inc
    ${PROJECT_SOURCE_DIR}/physics/inc
    ${PROJECT_SOURCE_DIR}/rendering/inc
    ${PROJECT_SOURCE_DIR}/ui/inc
    ${PROJECT_SOURCE_DIR}/water/inc
    ${PROJECT_SOURCE_DIR}/utils/inc
)

# Include all module directories
add_subdirectory(core)
add_subdirectory(world)
add_subdirectory(entities)
add_subdirectory(physics)
add_subdirectory(rendering)
add_subdirectory(ui)
add_subdirectory(water)
add_subdirectory(utils)

# Link libraries
target_link_libraries(PioterCraft ${FREETYPE_LIBRARIES} ${ASSIMP_LIBRARIES} OpenGL::GL glfw glad -lGL)

# Copy shader files to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders)
file(COPY ${CMAKE_SOURCE_DIR}/textures/ DESTINATION ${CMAKE_BINARY_DIR}/textures)
file(COPY ${CMAKE_SOURCE_DIR}/fonts/ DESTINATION ${CMAKE_BINARY_DIR}/fonts)

target_include_directories(PioterCraft PRIVATE ${PROJECT_SOURCE_DIR}/external/glad/include ${OPENGL_INCLUDE_DIR})
target_compile_options(PioterCraft PRIVATE -Wall -Werror -Wpedantic -Wshadow)
include(CTest)
enable_testing()

