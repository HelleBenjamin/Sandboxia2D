cmake_minimum_required(VERSION 3.10)

project(Sandboxia)

set(CMAKE_C_STANDARD 11)

# Project's headers
include_directories(include)

# Main files
set(SRC_FILES
    src/main.c
    src/game.c
    src/render.c
)


add_executable(Sandboxia ${SRC_FILES})
target_link_libraries(Sandboxia PRIVATE raylib m dl pthread)

# Set building flags
set(CMAKE_CFLAGS "${CMAKE_CFLAGS} -O0 -g -fsanitize=address") # Use -O2 or -O3 for better performance, may increase compile time, default is -O1, for debug use -O0 and -g
# To get the best performance use -march=native, may not work on all platforms

set_target_properties(Sandboxia PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Copies assets to build directory
add_custom_command(TARGET Sandboxia POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Sandboxia>/assets
)
