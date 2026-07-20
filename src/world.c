#include "../include/world.h"
#include "../include/main.h"
#include "../include/render.h"
#include <stdlib.h>

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
  float scale = 0.075f; // Controls smoothness (lower = smoother, higher = rougher)
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

  /*TODO: Generate caves, lakes, trees, etc..*/
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