# Sandboxia2D

Sandboxia is a sandbox game written in C++ using OpenGL. It is designed to be a simple sandbox game that is easy to run on older machines. The game is still in early development. OpenGL 3.3 will be supported in the future after the game's core development is complete.

## Features
- Bugs may be present
- 2D World
- OpenGL 2.1

## How to play

- W, A, S, D to move
- Left mouse button to destroy blocks
- Right mouse button to place blocks
- F1 to save the world

## Building
- Currently only Linux is supported
- Clone the repository
- Run `make` to build the game
- Run `./Sandboxia` to play the game

## Launch args

- `-w` to set the width of the window
- `-h` to set the height of the window
- `-v` to set the VSYNC, Default is 1
- `-c` to set the collision, Default is 1
- `-d` to set the debug, Default is 0

#### Example

`./Sandboxia -v 1 -d 1`

### For debug

Debug keys:

- P to print the player position
- O to print the selector position
- T to print the current tile data at the cursor
- F to print the current FPS

## License
Copyright (c) 2024-2025 Benjamin Helle

Sandboxia2D is licensed under the GPL-3.0 license.
