cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)

project(Sandboxia)

set(CMAKE_C_STANDARD 11)

# Project's headers
include_directories(include)

# Main files
set(SRC_FILES
    src/collision.c
    src/entity.c
    src/main.c
    src/mod.c
    src/player.c
    src/render.c
    src/world.c
)


add_executable(Sandboxia ${SRC_FILES})
target_link_libraries(Sandboxia PRIVATE raylib m dl pthread lua5.4)
# If gives lua5.4 error, change to "lua"

set(CMAKE_CFLAGS_DEBUG_INIT "-Wall -O0 -g -fsanitize=address") # Debug flags
set(CMAKE_CFLAGS_RELEASE_INIT "-Wall -O2") # Release flags
# Set building flags
# To get the best performance use -march=native, may not work on all platforms
set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} -Wall -O0 -g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O2")

set_target_properties(Sandboxia PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Copies assets to build directory
add_custom_command(TARGET Sandboxia POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Sandboxia>/assets
)
