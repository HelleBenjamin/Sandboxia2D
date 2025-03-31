#include "../include/Sandboxia/world.h"
#include "../include/Sandboxia/renderer.h"
#include <cmath>
#include <iostream>

using namespace std;

#define STB_PERLIN_IMPLEMENTATION
#include "../include/stb_perlin.h"

int WORLD_WIDTH = 256; // Default world size
int WORLD_HEIGHT = 128;

int tileCount = 10; // To get the placeable tiles, subtract 4 from this value
int PlaceableTileCount = tileCount-4;
int DefaultPlaceableTileCount = PlaceableTileCount;  

Tile DefaultTiles[0xFF] = { // Default values of tiles
/*  textureID, State, isSolid, tileID */
    {T_Player, 0, 0, 0},    //Player 
	{T_Player_Left, 0, 0, 0},    //Player Left
	{T_Player_Right, 0, 0, 0},    //Player Right
    {T_Selector, 0, 0, 0},  //Selector
    {T_Air, 0, 0, 0},       //Air
    {T_Grass, 0, 1, 0},     //Grass
    {T_Stone, 0, 1, 0},     //Stone
    {T_Dirt, 0, 1, 0},      //Dirt
    {T_Sand, 0, 1, 0},      //Sand
    {T_Wood, 0, 1, 0}       //Wood
};

void generateWorld(World& world, int seed) {
    if (seed == -1) { // If seed is -1, generate a random seed
        srand(time(NULL));
        world.seed = rand();
    } else {
        srand(seed);
        world.seed = seed;
    }

    // Set default world size
    world.height = WORLD_HEIGHT;
    world.width = WORLD_WIDTH;

    world.tiles = new Tile*[world.width];
    for (int x = 0; x < world.width; ++x) {
        world.tiles[x] = new Tile[world.height];
    }

    const int width = world.width;

    vector<int> terrainHeight;
    terrainHeight.resize(width);
    //int terrainHeight[width];

    // Perlin noise parameters
    float scale = 0.1f; // Controls smoothness (lower = smoother, higher = rougher)
    float amplitude = 10.0f; // Controls terrain height variation

    for (int x = 0; x < world.width; ++x) {
        float noise = stb_perlin_noise3(x * scale, world.seed * 0.1f, 0.0f, 0, 0, 0);
        int heightOffset = static_cast<int>(noise * amplitude);
        terrainHeight[x] = (world.height / 2) + heightOffset;
    }

    // Generate terrain based on height map
    for (int x = 0; x < world.width; ++x) {
        for (int y = 0; y < world.height; ++y) {
            int terrainY = world.height - 1 - y;
            if (terrainY > terrainHeight[x]) {
                world.tiles[x][y] = DefaultTiles[TypeAir];
            } else if (terrainY == terrainHeight[x]) {
                world.tiles[x][y] = DefaultTiles[TypeGrass];
            } else if (terrainY > terrainHeight[x] - 4) {
                world.tiles[x][y] = DefaultTiles[TypeDirt];
            } else {
                world.tiles[x][y] = DefaultTiles[TypeStone];
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

	// Free the old world
    for (int i = 0; i < world->width; i++) {
        delete[] world->tiles[i];
    }
    delete[] world->tiles;


	World newWorld; // Temporary world to load the world from file

	// Read the world dimensions
	fread(&newWorld.width, sizeof(int), 1, file);
    fread(&newWorld.height, sizeof(int), 1, file);

	// Now read the world seed
	fread(&newWorld.seed, sizeof(int), 1, file);

	// Read the world name
	fread(newWorld.name, sizeof(char), 32, file);

	// And lastly, read the tile matrix
	newWorld.tiles = new Tile * [newWorld.width];
	for (int x = 0; x < newWorld.width; ++x) {
		newWorld.tiles[x] = new Tile[newWorld.height];
		fread(newWorld.tiles[x], sizeof(Tile), newWorld.height, file);
	}

	*world = newWorld;

    log("[INFO] Loaded world");
    log("[INFO] World name: " + std::string(world->name));
    log("[INFO] World size: " + std::to_string(world->width) + "x" + std::to_string(world->height));
    log("[INFO] World seed: " + std::to_string(world->seed));

    fclose(file);
	WORLD_HEIGHT = world->height;
	WORLD_WIDTH = world->width;
}

void saveWorld(const char* filePath, const World* world) {

    FILE* file = fopen(filePath, "wb"); 

    if (!file) {
        log("[ERROR] Failed to open file for saving");
        return;
    }

	// Write the world size first, this way we can allocate memory for the tiles before reading them
	fwrite(&world->width, sizeof(int), 1, file);
	fwrite(&world->height, sizeof(int), 1, file);

	// Then write the world seed
	fwrite(&world->seed, sizeof(int), 1, file);

	// Write the world name
	fwrite(world->name, sizeof(char), 32, file);

    // Write the tile matrix
    for (int x = 0; x < world->width; ++x) {
        fwrite(world->tiles[x], sizeof(Tile), world->height, file);
    }

	// This way the world loading/saving is less error-prone

    log("[INFO] Saved world: " + std::string(world->name));

    fclose(file);
}