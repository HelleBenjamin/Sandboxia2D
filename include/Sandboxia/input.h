#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include "renderer.h"

void HandleInput(World &world, Player &player, Renderer &renderer, float deltaTime);

#endif
