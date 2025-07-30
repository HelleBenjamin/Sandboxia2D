#pragma once
#ifndef MAIN_H
#define MAIN_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#if defined (_WIN32)
#define GLFW_DLL
#endif
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool VSYNC;
extern bool COLLISION;
extern bool DEBUG;
extern bool MODS_ENABLED;
extern bool SOUNDS_ENABLED;
extern int LOG_LEVEL;

#define VERSION_STR "0.1.9-beta"
#define VERSION_NUM 19

std::string handleConsoleCommand(std::string command); // Returns the output of the command to the console

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int initGame();
int main(int argc, char *argv[]);
void log(int type, char *fmt, ...);

#define LOG_NULL  0
#define LOG_ERR   1 
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4
#define LOG_MOD   5

#endif