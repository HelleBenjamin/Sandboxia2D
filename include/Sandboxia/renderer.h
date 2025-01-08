#ifndef RENDERER_H
#define RENDERER_H
#include "main.h"
#include "player.h"

/* Modern renderer */

class Renderer {
public:
    void init();
    void exit();
    void loadTextures();
    void freeTextures();
    void clearScreen();
    void drawTile(Tile tile, int x, int y);

    unsigned int compileShader(GLenum type, const char* source);
    unsigned int createShaderProgram();

    void RenderViewport(Camera& camera, Player& player, Tile world[][WORLD_HEIGHT], GLFWwindow* window); 

private:
    GLuint AirTex, GrassTex, StoneTex, DirtTex, PlayerTex, SelectorTex;
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;
};

#endif