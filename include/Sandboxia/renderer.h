#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "../main.h"
#include "player.h"
#include "world.h"
#include <vector>

/* Modern renderer */

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
    T_Wood
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

private:
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;

};

extern Renderer renderer;

#endif