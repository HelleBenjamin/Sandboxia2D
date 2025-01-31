#include "../include/Sandboxia/renderer.h"
#include "../include/main.h"
#include <iostream>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

/* 
    OpenGL 2.1 Compatibility based renderer
    Uses immediate mode to draw tiles.
    Outdated! Please use the new renderer. Use this if must.
*/

void Renderer::init() {
    log("[RENDERER] Using 'Legacy' as the render engine");
    loadTextures(); 
}

GLuint loadTexture(const char* filepath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    if (!data) {
        log("[ERROR] Failed to load texture: " + string(filepath));
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glad_glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    log("[RENDERER] Loaded texture: '" + string(filepath) + "' with ID: " + to_string(textureID));
    return textureID;
}

void unloadTexture(GLuint textureID) {
    glDeleteTextures(1, &textureID);
    log("[RENDERER] Unloaded texture with ID: " + to_string(textureID));
}

void Renderer::loadTextures(){
    const char* filenames[] = {"assets/player.png", "assets/selector.png", "assets/air.png", "assets/grass.png", "assets/stone.png", "assets/dirt.png", "assets/sand.png"}; // The order must be the same as defined in world.h

    glGenTextures(tileCount, textures.data());

    for (int i = 0; i < tileCount; i++) {
        textures.push_back(loadTexture(filenames[i]));
        //textures[i] = loadTexture(filenames[i]);
    }
}

void Renderer::freeTextures(){
    for (int i = 0; i < textures.size(); i++) {
        unloadTexture(textures[i]);
    }
}

void Renderer::exit() {
    freeTextures();
}


void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawTile(Tile tile, int x, int y) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, textures[tile.type]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x * TILE_SIZE * SCALER, y * TILE_SIZE * SCALER);
    glTexCoord2f(0, 1); glVertex2f(x * TILE_SIZE * SCALER, (y + 1) * TILE_SIZE * SCALER);
    glTexCoord2f(1, 1); glVertex2f((x + 1) * TILE_SIZE * SCALER, (y + 1) * TILE_SIZE * SCALER);
    glTexCoord2f(1, 0); glVertex2f((x + 1) * TILE_SIZE * SCALER, y * TILE_SIZE * SCALER);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Renderer::RenderViewport(Camera& camera, Player& player, World& world, GLFWwindow* window) {
    //glClear(GL_COLOR_BUFFER_BIT);

    camera.posX += (player.posX - camera.posX);
    camera.posY += (player.posY - camera.posY);

    // Calculate the visible tiles
    float startX = (camera.posX - camera.width / TILE_SIZE / SCALER / 2);
    float startY = (camera.posY - camera.height / TILE_SIZE / SCALER / 2);
    float endX = startX + camera.width / TILE_SIZE / SCALER;
    float endY = startY + camera.height / TILE_SIZE / SCALER;

    // Render visible tiles
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            int tileX = (int)x;
            int tileY = (int)y;
            if (tileX >= 0 && tileX < world.width && tileY >= 0 && tileY < world.height && world.tiles[tileX][tileY].type != T_Air) {
                drawTile(world.tiles[tileX][tileY], x - startX, y - startY);
            }
        }
    }

    // Render the player at the center of the screen
    drawTile(player.playerTile, (camera.width / TILE_SIZE / SCALER / 2), camera.height / TILE_SIZE / SCALER / 2);

    // Render the selector
    drawTile(player.SelectorTile, (player.SelectorX / TILE_SIZE / SCALER), (player.SelectorY / TILE_SIZE / SCALER));

    //glfwSwapBuffers(window);
}