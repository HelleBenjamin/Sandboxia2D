#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "world.h"
#include <raylib.h>
#include <stdint.h>

typedef struct {
  Entity  player;
  Vector2 selector;
  uint8_t selected_tile;
} Player;

void update_player(Player* player, World* world, float dt);
void handle_input(Player* player, World* world, Camera2D* camera, float dt);

#endif