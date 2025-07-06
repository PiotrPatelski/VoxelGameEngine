# Physics module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CollisionDetector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Raycaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Frustum.cpp
)

# Add header files for IDE
set(PHYSICS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/CollisionDetector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Raycaster.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Frustum.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${PHYSICS_HEADERS})
