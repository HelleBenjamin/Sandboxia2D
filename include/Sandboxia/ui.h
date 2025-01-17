#pragma once
#ifndef UI_H
#define UI_H

#include "../main.h"
#include "player.h"
#include "renderer.h"
#include "world.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

extern bool isConsoleOpen;
extern bool isMenuOpen;

void InitUI(GLFWwindow* window);
void HandleUI(GLFWwindow* window, World &world, Player &player, Renderer &renderer);
void ConsoleUI();
void DebugUI(GLFWwindow* window, Player &player, World &world, Renderer &renderer);
void MenuUI(GLFWwindow* window ,World &world);
void ExitUI();


#endif