#include "../include/Sandboxia/world.h"
#include "../include/Sandboxia/renderer.h"
#include <cmath>
#include <iostream>

int WORLD_WIDTH = 128; // Default world size
int WORLD_HEIGHT = 128;

int tileCount = 7; // To get the placeable tiles, subtract 2

Tile DefaultTiles[0xFF] = { // Default values of tiles
/*  textureID, State, isSolid, tileID */
    {T_Player, 0, 0, 0},    //Player 
    {T_Selector, 0, 0, 0},  //Selector
    {T_Air, 0, 0, 0},       //Air
    {T_Grass, 0, 1, 0},     //Grass
    {T_Stone, 0, 1, 0},     //Stone
    {T_Dirt, 0, 1, 0},      //Dirt
    {T_Sand, 0, 1, 0},      //Sand
};

void generateWorld(World& world, int seed) {
    if (seed == -1) {
        srand(time(NULL));
        world.seed = rand();
    } else {
        srand(seed);
        world.seed = seed;
    }

    // Set world size
    world.height = WORLD_HEIGHT;
    world.width = WORLD_WIDTH;

    world.tiles = new Tile*[world.width];
    for (int x = 0; x < world.width; ++x) {
        world.tiles[x] = new Tile[world.height];
    }


    int terrainHeight[world.width];
    for (int x = 0; x < world.width; ++x) {
        double noise = sin((double)x / 16.0 + world.seed) * 8.0 + cos((double)x / 32.0 + world.seed) * 4.0; // Combine waves for variation
        terrainHeight[x] = (world.height / 2) + static_cast<int>(noise);
    }

    for (int x = 0; x < world.width; ++x) {
        for (int y = 0; y < world.height; ++y) {
            int terrainY = world.height - 1 - y;
            if (terrainY > terrainHeight[x]) {
                world.tiles[x][y] = DefaultTiles[TypeAir];
                //world.tiles[x][y].type = T_Air; // Air
                //world.tiles[x][y].isSolid = false;
            } else if (terrainY == terrainHeight[x]) {
                world.tiles[x][y] = DefaultTiles[TypeGrass];
                //world.tiles[x][y].type = T_Grass; // Grass
                //world.tiles[x][y].isSolid = true;
            } else if (terrainY > terrainHeight[x] - 4) {
                world.tiles[x][y] = DefaultTiles[TypeDirt];
                //world.tiles[x][y].type = T_Dirt; // Dirt
                //world.tiles[x][y].isSolid = true;
            } else {
                world.tiles[x][y] = DefaultTiles[TypeStone];
                //world.tiles[x][y].type = T_Stone; // Stone
                //world.tiles[x][y].isSolid = true;
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