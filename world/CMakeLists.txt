# World module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/World.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChunkCoord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChunkGraphics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChunkLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChunkUpdater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChunkVoxels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CpuChunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Cube.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LightPropagator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TreeGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GridGenerator.cpp
)

# Add header files for IDE
set(WORLD_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/World.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ChunkCoord.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ChunkGraphics.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ChunkLoader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ChunkUpdater.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ChunkVoxels.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/CpuChunk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/VoxelTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Cube.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/CubeData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/LightPropagator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/TreeGenerator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/GridGenerator.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${WORLD_HEADERS})
