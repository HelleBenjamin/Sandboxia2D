# Sandboxia2D

Sandboxia is a sandbox game written in C++ using OpenGL. It is designed to be a simple sandbox game that is easy to run on older machines. The game is still in early development. The render engine was upgraded to OpenGL 3.0. The 'Legacy' render engine is still included for backwards compatibility. Using ImGui for the GUI.

## Features
- Bugs may be present
- 2D World
- OpenGL 3.0
- VSYNC
- World saving/loading
- Debug mode

## How to play

- W, A, S, D to move
- Left mouse button to destroy blocks
- Right mouse button to place blocks
- T to open console

## Building
- Currently only Linux is supported
    1. Run `mkdir build`  
    2. Run `cmake -B build` This will generate the Makefile
    3. Run `cmake --build build -j(number of threads)` to build the game.
- Run `./build/Sandboxia` to play the game.

## Launch args

- `-w` to set the width of the window
- `-h` to set the height of the window
- `-v` to set the VSYNC, Default is 1
- `-c` to set the collision, Default is 1
- `-d` to set the debug, Default is 0

#### Example

`./Sandboxia -v 1 -d`

### For debug

Debug keys:

- P to print the player position
- O to print the selector position
- T to print the current tile data at the cursor
- F to print the current FPS


## Third-party libraries used
- [ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [GLAD](https://github.com/Dav1dde/glad)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm)


## License
Copyright (c) 2024-2025 Benjamin Helle

Sandboxia2D is licensed under the GPL-3.0 license.
