# Core module CMakeLists.txt

# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/App.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Clock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ThreadPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
)

# Add header files for IDE
set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/App.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Clock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/ThreadPool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Camera.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${CORE_HEADERS})
