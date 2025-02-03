# Sandboxia2D

Sandboxia is a sandbox game written in C++ using OpenGL. It is designed to be a simple sandbox game that is easy to run on older machines. The game is still in development. The render engine was upgraded to OpenGL 3.0. The 'Legacy' render engine is still included for backwards compatibility. Sandboxia2D comes with a simple API for mods to add new features to the game. For deeper modding, modify the source code directly. Windows support is still experimental.

## Features
- Bugs may be present
- 2D World
- OpenGL 3.0
- VSYNC
- World saving/loading
- Debug mode
- Console
- Support for mods!

## How to play

- W, A, S, D to move
- Left mouse button to destroy blocks
- Right mouse button to place blocks
- T to open console

## Building
- Currently only Linux is supported(Windows is still experimental)
- You can use WSL to run the game on Windows.
- Building instructions:
    1. Run `mkdir build`  
    2. Run `cmake -B build` This will generate the Makefile
    3. Run `cmake --build build -j(number of threads)` to build the game.
- Run `./build/Sandboxia` to play the game.

## To build a simple mod
- Mod building instructions:
    1. Create a `.cpp` file in `mods` folder
    2. Include `mod_api.h` in the mod file
    3. Compile with `g++ -shared -fPIC mod.cpp -o mod.so`. In windows replace `.so` with `.dll`.
- Mods will be loaded automatically.

## Launch args

- `-w` to set the width of the window
- `-h` to set the height of the window
- `-v` to set the VSYNC, Default is 1
- `-c` to set the collision, Default is 1
- `-d` to set the debug, Default is 0
- `-disableMods` to disable mods

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
- [stb_perlin](https://github.com/nothings/stb)


## License
Copyright (c) 2024-2025 Benjamin Helle

Sandboxia2D is licensed under the GPL-3.0 license.
