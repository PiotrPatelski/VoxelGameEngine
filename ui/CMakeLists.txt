# UI module CMakeLists.txt


# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Crosshair.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FontManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StatusTextRenderer.cpp
)

# Add header files for IDE
set(UI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Crosshair.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/FontManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/StatusTextRenderer.hpp
)

target_sources(${PROJECT_NAME} PRIVATE ${UI_HEADERS})
