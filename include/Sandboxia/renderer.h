#ifndef RENDERER_H
#define RENDERER_H
#include "main.h"
#include "player.h"

class Renderer {
public:
    void loadTextures();
    void freeTextures();
    void clearScreen();
    void swapBuffers();
    void drawTile(Tile tile, int x, int y);
    void drawPlayer(Player player, float x, float y);

    void RenderViewport(Camera& camera, Player& player, Tile world[][WORLD_HEIGHT], GLFWwindow* window); 

private:
    GLuint AirTex, GrassTex, StoneTex, DirtTex, PlayerTex, SelectorTex;
};

#endif