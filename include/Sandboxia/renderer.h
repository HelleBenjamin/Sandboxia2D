#ifndef RENDERER_H
#define RENDERER_H
#include "main.h"
#include "player.h"

/* Modern renderer */

#define TEXTURE_COUNT 6

enum TextureType {
    T_Air = 0,
    T_Grass = 1,
    T_Stone = 2,
    T_Dirt = 3,
    T_Player = 4,
    T_Selector = 5
};

class Renderer {
public:
    void init();
    void exit();
    void loadTextures();
    void freeTextures();
    void clearScreen();
    void drawTile(Tile tile, int x, int y);

    GLuint loadTexture(const char* filepath);
    void unloadTexture(GLuint textureID);

    unsigned int compileShader(GLenum type, const char* source);
    unsigned int createShaderProgram();

    void RenderViewport(Camera& camera, Player& player, Tile world[][WORLD_HEIGHT], GLFWwindow* window);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;

    GLuint textures[TEXTURE_COUNT] = {0};

};


#endif