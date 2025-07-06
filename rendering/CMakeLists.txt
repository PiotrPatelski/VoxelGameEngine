# Rendering module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TextureManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RenderableChunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RenderableWaterMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VertexDataBuilder.cpp
)

# Add header files for IDE
set(RENDERING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Renderer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Material.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/TextureManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/RenderableChunk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/RenderableWaterMesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/VertexData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/VertexDataBuilder.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${RENDERING_HEADERS})
