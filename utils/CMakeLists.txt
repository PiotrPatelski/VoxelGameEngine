# Utils module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeighborCubesGatherer.cpp
)

# Add header files for IDE
set(UTILS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/NeighborCubesGatherer.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${UTILS_HEADERS})
