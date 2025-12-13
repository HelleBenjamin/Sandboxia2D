#include "../include/main.h"
#include "../include/render.h"
#include "../include/game.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../include/stb_perlin.h"

TileConfig tile_configs[0xFF] = {
  /* Default tile config*/
  {TypeAir_ID, false, false, TypeAir_Texture , "air"},
  {TypeGrass_ID, true, true, TypeGrass_Texture, "grass"},
  {TypeStone_ID, true, true, TypeStone_Texture, "stone"},
  {TypeDirt_ID, true, true, TypeDirt_Texture, "dirt"},
  {TypeSand_ID, true, true, TypeSand_Texture, "sand"},
  {TypeWood_ID, true, true, TypeWood_Texture, "wood"},
  {TypeLeaves_ID, true, true, TypeLeaves_Texture, "leaves"},
};

int num_tile_configs = 6; /* Number of tile configs */


bool check_collision_box(World *world, float x, float y, float width, float height) {
  /* This checks all the bounds of the box*/
  float world_x = x / TILE_SIZE / RENDER_SCALE;
  float world_y = y / TILE_SIZE / RENDER_SCALE;
  float world_width = width / TILE_SIZE / RENDER_SCALE;
  float world_height = height / TILE_SIZE / RENDER_SCALE;
  
  int minX = (int)floorf(world_x);
  int minY = (int)floorf(world_y);
  int maxX = (int)floorf(world_x + world_width - 0.001f);
  int maxY = (int)floorf(world_y + world_height - 0.001f);
  
  for (int tileX = minX; tileX <= maxX; tileX++) {
    for (int tileY = minY; tileY <= maxY; tileY++) {
      if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT) return true; /* Out of bound check */
      if (tile_configs[world->tiles[translate_index(tileX, tileY)].type].is_solid) return true; /* Check if solid*/
    }
  }
  
  return false;
}

/* Old collision check, modified but still usefull :)*/
bool check_collision(World *world, float x, float y) {
  float world_x = x / TILE_SIZE / RENDER_SCALE; /* Do not use FULL_TILE_SIZE!! */
  float world_y = y / TILE_SIZE / RENDER_SCALE;
  int tileX = (int)floorf(world_x);
  int tileY = (int)floorf(world_y);

  TileConfig* config = &tile_configs[world->tiles[translate_index(tileX, tileY)].type];

  if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT) {
    return true;
  }
  return config->is_solid;
}

void handle_input(Player *player, World *world, Camera2D *camera, float dt) {
  float playerWidth = TILE_SIZE * RENDER_SCALE; /* Actual size(rendered size)*/
  float playerHeight = TILE_SIZE * RENDER_SCALE;

  /*
    Old selector
    int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
    int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset
  */

  player->selector = GetMousePosition();

  int new_selected_tile = (int)((float)(player->selected_tile) + roundf(GetMouseWheelMove()));
  if (new_selected_tile >= 0 && new_selected_tile <= num_tile_configs) player->selected_tile = new_selected_tile;

  Vector2 worldPos = GetScreenToWorld2D(player->selector, *camera); /* Simple conversion*/
  int selX = (int)(worldPos.x / (TILE_SIZE * RENDER_SCALE)); /* Use FULL_TILE_SIZE? */
  int selY = (int)(worldPos.y / (TILE_SIZE * RENDER_SCALE));
  
  Vector2 new_position = player->position;
  
  /* Player movement */
  if (IsKeyDown(KEY_W)) {
    new_position.y -= player->speed * dt;
    player->direction = 0;
  }
  if (IsKeyDown(KEY_S)) {
    new_position.y += player->speed * dt;
    player->direction = 0;
  }
  if (IsKeyDown(KEY_A)) {
    new_position.x -= player->speed * dt;
    player->direction = 1;
  }
  if (IsKeyDown(KEY_D)) {
    new_position.x += player->speed * dt;
    player->direction = 2;
  }
  if (IsKeyDown(KEY_SPACE)) {
    player->velocity.y = -JUMP_POWER;
    player->direction = 0;
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (selX >= 0 && selX < WORLD_WIDTH && selY >= 0 && selY < WORLD_HEIGHT) world->tiles[translate_index(selX, selY)].type = TypeAir_ID;
  }

  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    if (selX >= 0 && selX < WORLD_WIDTH && selY >= 0 && selY < WORLD_HEIGHT) world->tiles[translate_index(selX, selY)].type = player->selected_tile;
  }


  if (!check_collision_box(world, new_position.x, player->position.y, playerWidth, playerHeight)) {
    player->position.x = new_position.x;
  }
  
  if (!check_collision_box(world, player->position.x, new_position.y, playerWidth, playerHeight)) {
    player->position.y = new_position.y;
  }

  /* World loading/saving*/
  if (IsKeyPressed(KEY_F1)) {
    saveWorld("world.dat", world);
  }
  if (IsKeyPressed(KEY_F2)) {
    loadWorld("world.dat", world);
  }
}

void update_player(Player *player, World *world, float dt) {
  float playerWidth = TILE_SIZE * RENDER_SCALE;
  float playerHeight = TILE_SIZE * RENDER_SCALE;
  
  if (!player->onGround) player->accel.y = GRAVITY;
  else player->accel.y = 0;

  player->velocity.x += player->accel.x * dt;
  player->velocity.y += player->accel.y * dt;
  
  /* Enhanced functions*/
  float newX = player->position.x + player->velocity.x * dt;
  float newY = player->position.y + player->velocity.y * dt;
  
  /* Check if player is touching a wall, x-check*/
  if (!check_collision_box(world, newX, player->position.y, playerWidth, playerHeight)) {
    player->position.x = newX;
  } else {
    player->velocity.x = 0.0f;
  }
  
  /* Check if player is on the ground, y-check */
  if (!check_collision_box(world, player->position.x, newY, playerWidth, playerHeight)) {
    player->position.y = newY;
    player->onGround = false;
  } else {
    player->velocity.y = 0.0f;
    if (player->velocity.y > 0 || check_collision_box(world, player->position.x, player->position.y + 1, playerWidth, playerHeight)) {
      player->onGround = true;
    }
  }
}

int translate_index(int x, int y) {
  return x + y * WORLD_WIDTH;
}

void generateWorld(World* world, int seed) { // The best world generation ever :)
  if (seed == -1) { // If seed is -1, generate a random seed
    world->seed = rand();
  } else {
    srand(seed);
    world->seed = seed;
  }

  world->tiles = (Tile*)malloc(WORLD_SIZE * sizeof(Tile*));
  world->height = WORLD_HEIGHT;
  world->width = WORLD_WIDTH;

  int terrainHeight[WORLD_WIDTH];

  // Perlin noise parameters
  float scale = 0.1f; // Controls smoothness (lower = smoother, higher = rougher)
  float amplitude = 10.0f; // Controls terrain height variation

  for (int x = 0; x < WORLD_WIDTH; ++x) {
    float noise = stb_perlin_noise3(x * scale, world->seed * 0.1f, 0.0f, 0, 0, 0);
    int heightOffset = (int)(noise * amplitude);
    terrainHeight[x] = (WORLD_HEIGHT / 2) + heightOffset;
  }

  // Generate terrain based on height map
  for (int x = 0; x < WORLD_WIDTH; ++x) {
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
      int terrainY = WORLD_HEIGHT - 1 - y;
      if (terrainY > terrainHeight[x]) {
        world->tiles[translate_index(x, y)].type = TypeAir_ID;
      } else if (terrainY == terrainHeight[x]) {
        world->tiles[translate_index(x, y)].type = TypeGrass_ID;
      } else if (terrainY > terrainHeight[x] - 4) {
        world->tiles[translate_index(x, y)].type = TypeDirt_ID;
      } else {
        world->tiles[translate_index(x, y)].type = TypeStone_ID;
      }
    }
  }
}

int loadWorld(const char* filePath, World* world) {
  FILE* file = fopen(filePath, "rb");
  if (!file) {
    gamelog(GAMELOG_ERR,"Failed to open world '%s'", filePath);
    fclose(file);
    return 1;
  }

  // Free the old world
  if (world->tiles) {
    free(world->tiles);
    world->tiles = NULL;
  }

  uint32_t signature = 0;
  fread(&signature, sizeof(uint32_t), 1, file); // Read the world signature
  if (signature != WORLD_SIGNATURE) {
    gamelog(GAMELOG_ERR,"Invalid world signature! Expected %04x, got %04x", WORLD_SIGNATURE, signature);
    return 1;
  }

	/* Read the world data */
  fread(&world->width, sizeof(uint16_t), 1, file);
  fread(&world->height, sizeof(uint16_t), 1, file);
	fread(&world->seed, sizeof(uint32_t), 1, file);
	fread(world->name, sizeof(char), 32, file);


  world->tiles = (Tile*)malloc((world->width * world->height) * sizeof(Tile));

  if (!world->tiles) {
    gamelog(GAMELOG_ERR,"Failed to allocate memory for world");
    return 1;
  }

  fread(world->tiles, sizeof(Tile), world->width * world->height, file);


  gamelog(GAMELOG_INFO,"Loaded world '%s'", world->name);
  gamelog(GAMELOG_INFO,"World seed: %d", world->seed);

  fclose(file);

  return 0;
}

void saveWorld(const char* filePath, const World* world) {

  FILE* file = fopen(filePath, "wb"); // Save as a raw binary

  if (!file) {
    gamelog(GAMELOG_ERR," Failed to open file for saving");
    return;
  }

  int signature = WORLD_SIGNATURE; // Signature to check if the world is corrupted
  fwrite(&signature, sizeof(int), 1, file);

	/* Write the world data */
  fwrite(&world->width, sizeof(uint16_t), 1, file);
  fwrite(&world->height, sizeof(uint16_t), 1, file);
	fwrite(&world->seed, sizeof(uint32_t), 1, file);
	fwrite(world->name, sizeof(char), 32, file);
  fwrite(world->tiles, sizeof(Tile), WORLD_WIDTH*WORLD_HEIGHT, file);

  gamelog(GAMELOG_INFO,"Saved world '%s'", filePath);

  fclose(file);
}