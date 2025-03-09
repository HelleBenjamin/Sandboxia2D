#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include <vector>
#include <unordered_map>

/* Modern renderer */

enum TextureType {
    T_Player = 0,
    T_Selector = 1,
    T_Air = 2,
    T_Grass = 3,
    T_Stone = 4,
    T_Dirt = 5,
    T_Sand = 6,
    T_Wood = 7
};

class Renderer {
public:
    SDL_Renderer* renderer;
    SDL_Texture* loadTexture(const char* filepath);
    void unloadTexture(SDL_Texture* textureID);
    void initr(SDL_Window* window);
    void exitr();
    void loadTextures();
    void freeTextures();
    void clearScreen();
    void drawTile(Tile tile, int x, int y);

    std::vector<SDL_Texture*> textures;

    void RenderViewport(Camera& camera, Player& player, World& world, SDL_Window* window);
    void updateViewport(int width, int height);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;

};

extern Renderer renderer;

#endif