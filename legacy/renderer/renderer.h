#ifndef RENDERER_H
#define RENDERER_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include <vector>

/* Legacy renderer */

enum TextureType {
    T_Player = 0,
    T_Selector = 1,
    T_Air = 2,
    T_Grass = 3,
    T_Stone = 4,
    T_Dirt = 5,
    T_Sand = 6
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLuint loadTexture(const char* filepath);
void unloadTexture(GLuint textureID);
GLFWwindow* getCurrentWindow();

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

    std::vector<GLuint> textures;

    void RenderViewport(Camera& camera, Player& player, World& world, GLFWwindow* window);
    void updateViewport(int width, int height);

};

extern Renderer renderer;

#endif