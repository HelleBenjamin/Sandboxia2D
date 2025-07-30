#include "../include/Sandboxia/world.h"
#include "../include/Sandboxia/renderer.h"
#include <cmath>
#include <iostream>
#include <new>

using namespace std;

#define STB_PERLIN_IMPLEMENTATION
#include "../include/stb_perlin.h"

int WORLD_WIDTH = 256; // Default world size
int WORLD_HEIGHT = 128;

int tileCount = 11; // To get the placeable tiles, subtract 4 from this value
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
    {T_Wood, 0, 1, 0},      //Wood
    {T_Leaves, 0, 1, 0}
};

struct Tree { // Tree template, should make a template file which contains all the structures
    int tiles[3][5] = {
        {TypeAir,TypeAir, TypeLeaves, TypeLeaves, TypeAir},
        {TypeWood, TypeWood, TypeWood, TypeLeaves, TypeLeaves},
        {TypeAir, TypeAir, TypeLeaves, TypeLeaves, TypeAir}
    };
};


void generateWorld(World& world, int seed) { // The best world generation ever :)
    if (seed == -1) { // If seed is -1, generate a random seed
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
                int treeRand = rand() % 100;
                if (treeRand < 6) { // Chance of a tree
                    Tree tree;
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 5; j++) {
                            if (x + i >= 0 && x + i < world.width && y + j >= 0 && y + j < world.height) {
                                if (tree.tiles[i][j] == TypeAir) continue; // Skip empty tiles
                                world.tiles[x - i][y - j] = DefaultTiles[tree.tiles[i][j]];
                            }
                        }
                    }
                }
            } else if (terrainY > terrainHeight[x] - 4) {
                world.tiles[x][y] = DefaultTiles[TypeDirt];
            } else {
                world.tiles[x][y] = DefaultTiles[TypeStone];
            }
        }
    }
}

int loadWorld(const char* filePath, World* world) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        log(LOG_ERR,"Failed to open world '%s'", filePath);
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
        log(LOG_ERR,"Invalid world signature! Expected %04x, got %04x", WORLD_SIGNATURE, signature);
        return 1;
    }

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

    // Put the new world in place
	*world = newWorld;

    // Print new world stuff
    log(LOG_INFO,"Loaded world '%s'", world->name);
    log(LOG_INFO,"World size: %d x %d", world->width, world->height);
    log(LOG_INFO,"World seed: %d", world->seed);

    // Finish
    fclose(file);
	WORLD_HEIGHT = world->height;
	WORLD_WIDTH = world->width;

    return 0;
}

void saveWorld(const char* filePath, const World* world) {

    FILE* file = fopen(filePath, "wb"); // Save as a raw binary

    if (!file) {
        log(LOG_ERR," Failed to open file for saving");
        return;
    }

    int signature = WORLD_SIGNATURE; // Signature to check if the world is corrupted
    fwrite(&signature, sizeof(int), 1, file);

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

    log(LOG_INFO,"Saved world '%s'", filePath);

    fclose(file);
}