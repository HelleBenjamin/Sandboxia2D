#include "../include/main.h"
#include "../include/render.h"
#include "../include/game.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../include/stb_perlin.h"

bool check_collision(World *world, float x, float y) {
  int tileX = (int)(x / TILE_SIZE / RENDER_SCALE) +1; /* Need to divide due to the scaling */
  int tileY = (int)(y / TILE_SIZE / RENDER_SCALE) +1;
  if ((tileX >= 0) && (tileX < WORLD_WIDTH) && (tileY >= 0) && (tileY < WORLD_HEIGHT)) {
    return world->tiles[translate_index(tileX, tileY)].type != TypeAir;
  }
  return false;
}

void handle_input(Player *player, World *world, float dt) {
  Vector2 new_position = player->position;
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

  player->selector = GetMousePosition();

  if (!(check_collision(world, new_position.x, player->position.y)) && !(check_collision(world, player->position.x, new_position.y))) {
    player->position = new_position;
  }
}

void update_player(Player *player, World *world, float dt) {
  if (!player->onGround) player->accel.y = +GRAVITY;
  else player->accel.y = 0;

  player->velocity.x += player->accel.x * dt;
  player->velocity.y += player->accel.y * dt;
  player->position.x += player->velocity.x * dt;
  player->position.y += player->velocity.y * dt;

  if (check_collision(world, player->position.x, player->position.y+1)) {
    player->velocity.y = 0.0f;
    player->onGround = true;
  } else player->onGround = false;
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

  world->tiles = (Tile*)malloc((WORLD_WIDTH * WORLD_HEIGHT) * sizeof(Tile*));

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
        world->tiles[translate_index(x, y)].type = TypeAir;
      } else if (terrainY == terrainHeight[x]) {
        world->tiles[translate_index(x, y)].type = TypeGrass;
      } else if (terrainY > terrainHeight[x] - 4) {
        world->tiles[translate_index(x, y)].type = TypeDirt;
      } else {
        world->tiles[translate_index(x, y)].type = TypeStone;
      }
    }
  }
}

/*int loadWorld(const char* filePath, World* world) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        gamelog(GAMELOG_ERR,"Failed to open world '%s'", filePath);
        return 1;
    }

    // Free the old world
    if (world->tiles) {
        for (int i = 0; i < world->width; i++) {
            delete[] world->tiles[i];
        }

        delete[] world->tiles;
    }

	World newWorld = {0}; // Temporary world to load the world from file

    int signature = 0;
    fread(&signature, sizeof(int), 1, file); // Read the world signature
    if (signature != WORLD_SIGNATURE) {
      gamelog(GAMELOG_ERR,"Invalid world signature! Expected %04x, got %04x", WORLD_SIGNATURE, signature);
      return 1;
    }

	// Read the world dimensions
    fread(&newWorld.width, sizeof(int), 1, file);
    fread(&newWorld.height, sizeof(int), 1, file);

	// Now read the world seed
	fread(&newWorld.seed, sizeof(int), 1, file);

	// Read the world name
	fread(newWorld.name, sizeof(char), 32, file);

	for (int x = 0; x < newWorld.width; ++x) {
		newWorld.tiles[x] = new Tile[newWorld.height];
		fread(newWorld.tiles[x], sizeof(Tile), newWorld.height, file);
	}

    // Put the new world in place
	*world = newWorld;

  free(newWorld.tiles);

  // Print new world stuff
  gamelog(GAMELOG_INFO,"Loaded world '%s'", world->name);
  gamelog(GAMELOG_INFO,"World seed: %d", world->seed);

  // Finish
  fclose(file);

  return 0;
}*/

/*void saveWorld(const char* filePath, const World* world) {

  FILE* file = fopen(filePath, "wb"); // Save as a raw binary

  if (!file) {
    gamelog(GAMELOG_ERR," Failed to open file for saving");
    return;
  }

  int signature = WORLD_SIGNATURE; // Signature to check if the world is corrupted
  fwrite(&signature, sizeof(int), 1, file);

	// Then write the world seed
	fwrite(&world->seed, sizeof(int), 1, file);

	// Write the world name
	fwrite(world->name, sizeof(char), 32, file);

  // Write the tile matrix
  for (int x = 0; x < WORLD_WIDTH*WORLD_HEIGHT; ++x) {
    fwrite(world->tiles[x], sizeof(Tile), 1, file);
  }

  // This way the world loading/saving is less error-prone

  gamelog(GAMELOG_INFO,"Saved world '%s'", filePath);

  fclose(file);
}*/