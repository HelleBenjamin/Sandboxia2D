#ifndef RENDER_H
#define RENDER_H
#include "world.h"
#include "player.h"
#include <raylib.h>
#include <stdint.h>

enum TextureIDs {
  TypePlayer_Texture = 0,
  TypePlayer_Left_Texture,
  TypePlayer_Right_Texture,
  TypeSelector_Texture,
  TypeAir_Texture, /* Placeable tiles start here */
  TypeGrass_Texture,
  TypeStone_Texture,
  TypeDirt_Texture,
  TypeSand_Texture,
  TypeWood_Texture,
  TypeLeaves_Texture,
};


extern Texture2D textures[0xFF];
extern uint8_t num_textures;

int render_init();
int render_exit();

void render_game(Camera2D* camera, Player* player, World* world);

#endif