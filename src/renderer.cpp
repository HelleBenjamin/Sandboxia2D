#include "../include/Sandboxia/renderer.h"
#include "../include/main.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <iostream>
#include <raylib.h>
#include <vector>

using namespace std;
using namespace glm;

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

/* 
    Raylib-based renderer
*/

// Vertex and fragment shaders


void Renderer::init() {
    // I do understand this :)
    log(LOG_INFO,"[RENDERER] Using 'Modern' as the render engine");
    SetTargetFPS(60);
    InitWindow(800, 600, "Sandboxia2D");
    DisableCursor();

    loadTextures();
}


void Renderer::loadTextures(){
    const char* path = "assets/textures/"; // Texture directory
    const char* filenames[] = {"player.png", "player_left.png", "player_right.png", "selector.png", "air.png", "grass.png", "stone.png", "dirt.png", "sand.png", "wood.png", "leaves.png"}; // The order must be the same as defined in world.h

    textures.resize(tileCount); // Allocate memory

    for (int i = 0; i < tileCount; i++) {
        char filepath[256];
        snprintf(filepath, sizeof(filepath), "%s%s", path, filenames[i]);
        textures[i] = LoadTexture(filepath);
        
        // Improves quality
        SetTextureFilter(textures[i], TEXTURE_FILTER_POINT);
    }
}

void Renderer::freeTextures(){
    for (int i = 0; i < textures.size(); i++) {
        UnloadTexture(textures[i]);
    }
}

void Renderer::exit() {

    freeTextures();
    CloseWindow();
}

void Renderer::drawTile(Tile tile, int x, int y) {
    if (tile.type >= tileCount) return;
    
    Rectangle source = { 0, 0, (float)textures[tile.type].width, (float)textures[tile.type].height };
    Rectangle dest = { 
        (float)(x * TILE_SIZE * SCALER), 
        (float)(y * TILE_SIZE * SCALER), 
        TILE_SIZE * SCALER, 
        TILE_SIZE * SCALER 
    };
    Vector2 origin = { 0, 0 };
    
    DrawTexturePro(textures[tile.type], source, dest, origin, 0.0f, WHITE);
}

void Renderer::drawTilef(Tile tile, float x, float y) {
    if (tile.type >= tileCount) return;
    
    Rectangle source = { 0, 0, (float)textures[tile.type].width, (float)textures[tile.type].height };
    Rectangle dest = { 
        x * TILE_SIZE * SCALER, 
        y * TILE_SIZE * SCALER, 
        TILE_SIZE * SCALER, 
        TILE_SIZE * SCALER 
    };
    Vector2 origin = { 0, 0 };
    
    DrawTexturePro(textures[tile.type], source, dest, origin, 0.0f, WHITE);
}

void Renderer::drawTileWorld(Tile tile, float worldX, float worldY) {
    if (tile.type >= tileCount) return;
    
    Rectangle source = { 0, 0, (float)textures[tile.type].width, (float)textures[tile.type].height };
    Rectangle dest = { 
        worldX * TILE_SIZE * SCALER, 
        worldY * TILE_SIZE * SCALER, 
        TILE_SIZE * SCALER, 
        TILE_SIZE * SCALER 
    };
    Vector2 origin = { 0, 0 };
    
    DrawTexturePro(textures[tile.type], source, dest, origin, 0.0f, WHITE);
}

void Renderer::updateCamera(Player& player) {
    // Deadzone and following camera
    float deadzoneWidth = static_cast<float>(SCREEN_WIDTH) / TILE_SIZE / SCALER * 0.25f;
    float deadzoneHeight = static_cast<float>(SCREEN_HEIGHT) / TILE_SIZE / SCALER * 0.25f;
    float smoothness = 0.15f;
    
    // Player position in world coordinates
    float playerWorldX = player.posX * TILE_SIZE * SCALER;
    float playerWorldY = player.posY * TILE_SIZE * SCALER;
    
    // Calculate target based on deadzone
    Vector2 targetPos = camera.target;
    
    float playerRelX = playerWorldX - camera.target.x;
    float playerRelY = playerWorldY - camera.target.y;
    
    float deadzonePixelsX = deadzoneWidth * TILE_SIZE * SCALER;
    float deadzonePixelsY = deadzoneHeight * TILE_SIZE * SCALER;
    
    if (playerRelX > deadzonePixelsX) {
        targetPos.x = playerWorldX - deadzonePixelsX;
    } else if (playerRelX < -deadzonePixelsX) {
        targetPos.x = playerWorldX + deadzonePixelsX;
    }
    
    if (playerRelY > deadzonePixelsY) {
        targetPos.y = playerWorldY - deadzonePixelsY;
    } else if (playerRelY < -deadzonePixelsY) {
        targetPos.y = playerWorldY + deadzonePixelsY;
    }
    
    // Smooth interpolation
    camera.target.x += (targetPos.x - camera.target.x) * smoothness;
    camera.target.y += (targetPos.y - camera.target.y) * smoothness;
}

void Renderer::RenderViewport(Player& player, World& world) {
    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    // Update camera before rendering
    updateCamera(player);
    
    BeginMode2D(camera);
    
    // Calculate visible tiles in world coordinates
    Vector2 topLeft = GetScreenToWorld2D({0, 0}, camera);
    Vector2 bottomRight = GetScreenToWorld2D({(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, camera);
    
    int startX = (int)floor(topLeft.x / (TILE_SIZE * SCALER));
    int startY = (int)floor(topLeft.y / (TILE_SIZE * SCALER));
    int endX = (int)ceil(bottomRight.x / (TILE_SIZE * SCALER));
    int endY = (int)ceil(bottomRight.y / (TILE_SIZE * SCALER));
    
    // Render visible tiles
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            if (x >= 0 && x < world.width && 
                y >= 0 && y < world.height && 
                world.tiles[x][y].type != T_Air) {
                drawTileWorld(world.tiles[x][y], x, y);
            }
        }
    }
    
    // Render the player in world coordinates
    Tile playerDisplayTile;
    if (player.isMovingRight) {
        playerDisplayTile.type = TypePlayer_Right;
    } else if (player.isMovingLeft) {
        playerDisplayTile.type = TypePlayer_Left;
    } else {
        playerDisplayTile = player.playerTile;
    }
    
    drawTileWorld(playerDisplayTile, player.posX, player.posY);
    
    // Render the selector
    drawTileWorld(player.SelectorTile, 
                    player.SelectorX / TILE_SIZE / SCALER, 
                    player.SelectorY / TILE_SIZE / SCALER);
    
    EndMode2D();
    
    // Draw UI elements here (outside camera) if needed
    
    EndDrawing();
}
