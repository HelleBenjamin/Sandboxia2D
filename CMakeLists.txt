cmake_minimum_required(VERSION 3.10)

project(Sandboxia)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project's headers
include_directories(include)

# Main files
set(SRC_FILES
    src/main.cpp
    src/Renderer.cpp
    src/Player.cpp
    src/UI.cpp
)

# Third-party libraries
set(LIBS_FILES
    src/glad/glad.c
    src/imgui/imgui.cpp
    src/imgui/imgui_impl_glfw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
)

add_executable(Sandboxia ${SRC_FILES} ${LIBS_FILES})

if(WIN32)
    target_include_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/win32)
    target_link_libraries(Sandboxia PRIVATE opengl32)
endif()

if(UNIX)
    target_include_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/linux)
    target_link_libraries(Sandboxia PRIVATE GL glfw)
endif()


set_target_properties(Sandboxia PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})