#pragma once
#ifndef MAIN_H
#define MAIN_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL3/SDL.h"

#include <string>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool VSYNC;
extern bool COLLISION;
extern bool DEBUG;
extern bool MODS_ENABLED;
extern bool SHOULD_EXIT;

#define VERSION "0.1.6-beta SDL3-3.2.8"
#define SANDBOXIA_VERSION_NUM 016

std::string handleConsoleCommand(std::string command); // Returns the output of the command to the console

int initGame();
int main(int argc, char *argv[]);
void log(std::string msg);


#endif