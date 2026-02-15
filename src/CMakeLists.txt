# Collect source files
file(GLOB_RECURSE PROJECT_SRC
    *.cpp
)

add_executable(BABATALAL ${PROJECT_SRC})

target_include_directories(BABATALAL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

target_link_libraries(BABATALAL PRIVATE
    imgui
    glfw
    glad 
    OpenGL::GL
)

# macOS frameworks
if(APPLE)
    target_link_libraries(BABATALAL PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
