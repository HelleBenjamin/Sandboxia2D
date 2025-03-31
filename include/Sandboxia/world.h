#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "../main.h"

#define TILE_SIZE 8 // 8x8 pixels per tile
#define SCALER 5 // Scaler for tile size

// WORLD SIZE
//#define WORLD_WIDTH 128
//#define WORLD_HEIGHT 128

extern int WORLD_WIDTH;
extern int WORLD_HEIGHT;

typedef struct { // Tile structure
    int type;       // Type
    bool tileState; // Tile state, eg. is lit
    bool isSolid;   // Is solid
    int tileID;     // Tile ID
} Tile;

typedef struct {
    char name[32];      // World name
    int seed;           // World seed
    int width, height;  // World width and height
    Tile** tiles;       // Matrix to store tiles
} World;

enum TileType {
    TypePlayer = 0,
	TypePlayer_Left,
	TypePlayer_Right,
    TypeSelector,
    TypeAir,
    TypeGrass,
    TypeStone,
    TypeDirt,
    TypeSand,
    TypeWood,
    TypeLeaves,
};

extern int tileCount; // Keeps track of tile count
extern int PlaceableTileCount; // Keeps track of placeable tile count
extern int DefaultPlaceableTileCount; // Default placeable tile count

// Max tile count: 256
extern Tile DefaultTiles[0xFF];

/* Default tile values. 
    !Texture loading order is important!
    Keep it same as below.

    0: Player
	1: Player Left
	2: Player Right
    3: Selector
    4: Air
    5: Grass
    6: Stone
    7: Dirt
    8: Sand
    9: Wood
    10: Leaves
*/

void generateWorld(World& world, int seed);
void saveWorld(const char* filePath, const World* world);
void loadWorld(const char* filePath, World* world);

extern World world;

#endif