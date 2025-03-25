#pragma once
#ifndef MAIN_H
#define MAIN_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool VSYNC;
extern bool COLLISION;
extern bool DEBUG;
extern bool MODS_ENABLED;

#define VERSION "0.1.6-beta"
#define SANDBOXIA_VERSION_NUM 016

std::string handleConsoleCommand(std::string command); // Returns the output of the command to the console

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int initGame();
int main(int argc, char *argv[]);
void log(std::string msg);


#endif