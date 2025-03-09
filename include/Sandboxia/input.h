#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include "ui.h"

void InputHandler(SDL_Window* window, SDL_Event* event, Player& player,Camera& camera, World& world, float deltaTime);
void InputHandlerUI(SDL_Window* window, SDL_Event* event, Player& player, Camera& camera, World& world, float deltaTime);
void scrollCallback(SDL_Window* window, double xoffset, double yoffset);
void MouseEvent(SDL_Window* window, SDL_Event* event,Player& player, Camera& camera, World& world, float deltaTime);
#endif
