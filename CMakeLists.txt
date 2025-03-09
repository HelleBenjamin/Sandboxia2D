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

file(GLOB IMGUI_SOURCES src/imgui/*.cpp)
file(GLOB SDL3_img_SOURCES src/SDL3_image/*.c)
add_executable(Sandboxia ${SRC_FILES} ${IMGUI_SOURCES} ${SDL3_img_SOURCES})

#add_executable(Sandboxia ${SRC_FILES} ${LIBS_FILES})

if(UNIX)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    target_link_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/lib/linux/aarch64)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_link_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/lib/linux/x86_64)
  endif()
  target_link_libraries(Sandboxia PRIVATE GL SDL3)
elseif(WIN32)
  target_link_directories(Sandboxia PRIVATE ${CMAKE_SOURCE_DIR}/lib)
  target_link_libraries(Sandboxia PRIVATE win32 gdi32 opengl32 SDL3 SDL3_image)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall") # Optimize build

set_target_properties(Sandboxia PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})