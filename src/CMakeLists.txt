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

# windows dll issue fixes for windows
if(WIN32)
    set_target_properties(BABATALAL PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++ -static"
    )
    # Link Windows system libs explicitly to avoid dynamic lookups
    target_link_libraries(BABATALAL PRIVATE gdi32 winmm imm32)
endif()


# macOS frameworks
if(APPLE)
    target_link_libraries(BABATALAL PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
