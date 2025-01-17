#ifndef RENDERER_H
#define RENDERER_H
#include "../main.h"
#include "player.h"

/* Modern renderer */

#define TEXTURE_COUNT 7

enum TextureType {
    T_Air = 0,
    T_Grass = 1,
    T_Stone = 2,
    T_Dirt = 3,
    T_Player = 4,
    T_Selector = 5,
    T_Sand = 6
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


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

    GLuint textures[TEXTURE_COUNT] = {0};

    void RenderViewport(Camera& camera, Player& player, World& world, GLFWwindow* window);
    void updateViewport(int width, int height);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;

};


#endif