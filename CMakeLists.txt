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
    src/renderer.cpp
    src/player.cpp
    src/ui.cpp
    src/input.cpp
    src/world.cpp
    src/mod_api.cpp
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

if(UNIX)
    target_link_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(Sandboxia PRIVATE GL glfw3)
elseif(WIN32)
    target_link_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(Sandboxia PRIVATE win32 gdi32 opengl32 glfw)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1") # Use -O2 or -O3 for better performance, may increase compile time, default is -O1, for debug use -O0

set_target_properties(Sandboxia PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})