#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include "ui.h"

void InputHandler(GLFWwindow* window, Player& player,Camera& camera, World& world, float deltaTime);
void InputHandlerUI(GLFWwindow* window, Player& player, Camera& camera, World& world, float deltaTime);

#endif
