#include "../include/Sandboxia/world.h"
#include "../include/Sandboxia/renderer.h"
#include <cmath>
#include <iostream>

void generateWorld(World& world) {
    if (world.seed == 0) {
        srand(time(NULL));
        world.seed = rand();
    } else srand(world.seed);


    int terrainHeight[WORLD_WIDTH];
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        double noise = sin((double)x / 16.0 + world.seed) * 8.0 + cos((double)x / 32.0 + world.seed) * 4.0; // Combine waves for variation
        terrainHeight[x] = (WORLD_HEIGHT / 2) + static_cast<int>(noise);
    }

    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            int terrainY = WORLD_HEIGHT - 1 - y;
            if (terrainY > terrainHeight[x]) {
                world.tiles[x][y].type = T_Air; // Air
                world.tiles[x][y].isSolid = false;
            } else if (terrainY == terrainHeight[x]) {
                world.tiles[x][y].type = T_Grass; // Grass
                world.tiles[x][y].isSolid = true;
            } else if (terrainY > terrainHeight[x] - 4) {
                world.tiles[x][y].type = T_Dirt; // Dirt
                world.tiles[x][y].isSolid = true;
            } else {
                world.tiles[x][y].type = T_Stone; // Stone
                world.tiles[x][y].isSolid = true;
            }
        }
    }
}

void loadWorld(const char* filePath, World* world) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        log("[ERROR] Failed to open file for loading");
        return;
    }
    fread(world, sizeof(World), 1, file); // Read the entire World structure
    log("[INFO] Loaded world");
    log("[INFO] World name: " + std::string(world->name));
    log("[INFO] World size: " + std::to_string(world->width) + "x" + std::to_string(world->height));
    log("[INFO] World seed: " + std::to_string(world->seed));
    fclose(file);
}

void saveWorld(const char* filePath, const World* world) {
    FILE* file = fopen(filePath, "wb");
    if (!file) {
        log("[ERROR] Failed to open file for saving");
        return;
    }
    fwrite(world, sizeof(World), 1, file); // Write the entire World structure
    log("[INFO] Saved world");
    fclose(file);
}