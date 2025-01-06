#ifndef MAIN_H
#define MAIN_H

#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool VSYNC;
extern bool COLLISION;
extern bool DEBUG;

#define VERSION "0.1.2-beta"

#define TILE_SIZE 8 // 8x8 pixels per tile

#define WORLD_SIZE 64 // 64x64 tiles
#define SCALER 5 // Scaler for tile size

// WORLD SIZE
#define WORLD_WIDTH 128
#define WORLD_HEIGHT 128

typedef struct {
    int type;
    bool isVisible;
    bool tileState;
    bool isSolid;
    int tileID;
} Tile;

extern Tile world[WORLD_WIDTH][WORLD_HEIGHT];

void saveWorld(Tile world[][WORLD_HEIGHT], std::string filename);
void loadWorld(Tile (&world)[WORLD_WIDTH][WORLD_HEIGHT], std::string filename);

GLuint loadTexture(const char* filepath);
void unloadTexture(GLuint textureID);

int main(int argc, char *argv[]);
void log(std::string msg);


#endif