#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "../main.h"

#define TILE_SIZE 8 // 8x8 pixels per tile
#define SCALER 5 // Scaler for tile size

// WORLD SIZE
#define WORLD_WIDTH 128
#define WORLD_HEIGHT 128

typedef struct { // Tile structure
    int type;
    bool tileState;
    bool isSolid;
    int tileID;
} Tile;

typedef struct {
    char name[32]; // World name
    int seed; // World seed
    int width, height;
    Tile tiles[WORLD_WIDTH][WORLD_HEIGHT];
} World;

void generateWorld(World& world);
void saveWorld(const char* filePath, const World* world);
void loadWorld(const char* filePath, World* world);

#endif