# Entities module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/EntityManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BodyPart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicsComponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hitbox.cpp
)

# Add header files for IDE
set(ENTITIES_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Entity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/EntityManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/BodyPart.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/SteveData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/PhysicsComponent.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Hitbox.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${ENTITIES_HEADERS})
