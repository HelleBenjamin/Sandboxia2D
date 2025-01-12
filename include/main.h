#ifndef MAIN_H
#define MAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool VSYNC;
extern bool COLLISION;
extern bool DEBUG;

#define VERSION "0.1.3-beta"

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

void saveWorld(const char* filePath, const World* world);
void loadWorld(const char* filePath, World* world);

int main(int argc, char *argv[]);
void log(std::string msg);


#endif