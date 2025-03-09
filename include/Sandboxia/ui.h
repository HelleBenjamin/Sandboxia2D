#pragma once
#ifndef UI_H
#define UI_H

#include "../main.h"
#include "player.h"
#include "renderer.h"
#include "world.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl3.h"
#include "../imgui/imgui_impl_opengl3.h"

extern bool isConsoleOpen;
extern bool isMenuOpen;

void InitUI(SDL_Window* window);
void HandleUI(SDL_Window* window, World &world, Player &player, Renderer &renderer);
void ConsoleUI();
void DebugUI(SDL_Window* window, Player &player, World &world, Renderer &renderer);
void MenuUI(SDL_Window* window ,World &world);
void ExitUI();


#endif