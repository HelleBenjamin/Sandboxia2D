#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include <raylib.h>
#include <raymath.h>
#include <vector>

/* Modern renderer */

#define TILE_SIZE 8 // 8x8 pixels per tile
#define SCALER 5 // Scaler for tile size

enum TextureType {
    T_Player = 0,
	T_Player_Left,
	T_Player_Right,
    T_Selector,
    T_Air,
    T_Grass,
    T_Stone,
    T_Dirt,
    T_Sand,
    T_Wood,
    T_Leaves
};

class Renderer {
public:
    void init();
    void exit();
    void loadTextures();
    void freeTextures();
    void clearScreen();
    void drawTile(Tile tile, int x, int y);
    void drawTilef(Tile tile, float x, float y);

    void updateCamera(Player& player);
    void drawTileWorld(Tile tile, float worldX, float worldY);

    std::vector<Texture2D> textures;

    void RenderViewport(Player& player, World& world);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;

};

extern Renderer renderer;

#endif