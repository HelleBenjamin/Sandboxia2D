#include "../include/Sandboxia/renderer.h"
#include "../include/main.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/SDL3_image/SDL_image.h"
#include <iostream>
#include <vector>


using namespace std;

//#define STB_IMAGE_IMPLEMENTATION
//#include "../include/stb_image.h"

using namespace glm;

/* 
    OpenGL 3.0 Core based renderer
    Modern and fast compared to the legacy renderer.
    Use this as default.
*/

void Renderer::initr(SDL_Window* window) {
    log("[RENDERER] Using 'Modern' as the render engine");
    loadTextures();

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        log("[RENDERER] Failed to create renderer");
        exit(1);
    }
}

SDL_Texture* Renderer::loadTexture(const char* filepath) {
    if (!filepath) {
        log("[ERROR] Texture path is null");
        return nullptr;
    }

    SDL_Surface* surface = IMG_LoadPNG_IO(SDL_IOFromFile(filepath, "r"));
    if (!surface) {
        log("[ERROR] Failed to load texture: " + string(filepath));
        log("[ERROR] SDL_image error: " + string(SDL_GetError()));
        return nullptr;
    }


    SDL_Texture* textureID = SDL_CreateTextureFromSurface(renderer, surface);
    //SDL_FreeSurface(surface);
    SDL_DestroySurface(surface);

    if (!textureID) {
        log("[ERROR] Failed to create texture from surface: " + string(filepath));
        return nullptr;
    }

    log("[INFO] Loaded texture: '" + string(filepath) + "'");
    return textureID;
}

void Renderer::unloadTexture(SDL_Texture* textureID) {
    SDL_DestroyTexture(textureID);
    log("[INFO] Unloaded texture");
}

void Renderer::loadTextures(){
    const char* filenames[] = {"assets/player.png", "assets/selector.png", "assets/air.png", "assets/grass.png", "assets/stone.png", "assets/dirt.png", "assets/sand.png", "assets/wood.png"}; // The order must be the same as defined in world.h

    for (int i = 0; i < tileCount; i++) {
        textures.push_back(loadTexture(filenames[i]));
        //textures[i] = loadTexture(filenames[i]);
    }
}

void Renderer::freeTextures(){
    for (int i = 0; i < textures.size(); i++) {
        unloadTexture(textures[i]);
    }
    textures.clear();
}

void Renderer::exitr() {
    freeTextures();
}

void Renderer::drawTile(Tile tile, int x, int y) {
    SDL_FRect rect;
    rect.x = x * TILE_SIZE;
    rect.y = y * TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    SDL_Texture* texture = textures[tile.type];
    SDL_RenderTextureTiled(renderer, textures[tile.type], &rect, 1, NULL);
}

void Renderer::RenderViewport(Camera& camera, Player& player, World& world, SDL_Window* window) {
    //glClear(GL_COLOR_BUFFER_BIT);

    SDL_SetRenderDrawColor(renderer, 57, 155, 236, 1);
    SDL_RenderClear(renderer);

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

    SDL_RenderPresent(renderer);

    //glfwSwapBuffers(window);
}
