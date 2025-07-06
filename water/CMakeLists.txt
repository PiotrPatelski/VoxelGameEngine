# Water module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WaterMeshBuilder.cpp
)

# Add header files for IDE
set(WATER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/WaterSystem.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/WaterMeshBuilder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/CpuWaterMesh.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${WATER_HEADERS})
