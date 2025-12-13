#ifndef RENDER_H
#define RENDER_H
#include "game.h"

#define RENDER_SCALE 5.0f
#define TILE_SIZE 8
#define FULL_TILE_SIZE TILE_SIZE * RENDER_SCALE

extern Texture2D textures[0xFF];
extern uint8_t num_textures;

int render_init();
int render_exit();

void render_game(Camera2D* camera, Player* player, World* world);

#endif