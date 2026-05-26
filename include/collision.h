#ifndef COLLISION_H
#define COLLISION_H

#include "game.h"
#include "world.h"
#include <stdbool.h>

bool check_collision_box(World *world, float x, float y, float width, float height);
uint8_t check_collision_mask(World *world, float x, float y, float w, float h);

#endif