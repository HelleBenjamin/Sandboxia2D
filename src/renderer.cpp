#include "../include/Sandboxia/renderer.h"
#include "../include/main.h"
#include <iostream>

void Renderer::loadTextures(){
    AirTex = loadTexture("assets/air.png");
    GrassTex = loadTexture("assets/grass.png");
    StoneTex = loadTexture("assets/stone.png");
    DirtTex = loadTexture("assets/dirt.png");
    PlayerTex = loadTexture("assets/player.png");
    SelectorTex = loadTexture("assets/selection.png");
}

void Renderer::freeTextures(){
    unloadTexture(AirTex);
    unloadTexture(GrassTex);
    unloadTexture(StoneTex);
    unloadTexture(PlayerTex);
    unloadTexture(DirtTex);
    unloadTexture(SelectorTex);
}

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawTile(Tile tile, int x, int y) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    switch (tile.type) { // Bind right texture according to tile type
        case 0:
            glBindTexture(GL_TEXTURE_2D, AirTex);
            break;
        case 1:
            glBindTexture(GL_TEXTURE_2D, GrassTex);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, StoneTex);
            break;
        case 3:
            glBindTexture(GL_TEXTURE_2D, PlayerTex);
            break;
        case 4:
            glBindTexture(GL_TEXTURE_2D, DirtTex);
            break;
        case 5:
            glBindTexture(GL_TEXTURE_2D, SelectorTex);
            break;
        default:
            log("[ERROR] Invalid tile texture type: " + std::to_string(tile.type));
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
    }
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

void Renderer::drawPlayer(Player player, float x, float y) { // Now obsolete
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, PlayerTex);
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

void Renderer::RenderViewport(Camera& camera, Player& player, Tile world[][WORLD_HEIGHT], GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);
    // Update the camera position to follow the player
    camera.posX += (player.posX - camera.posX);
    camera.posY += (player.posY - camera.posY);

    // Calculate the visible tiles
    float startX = (camera.posX - camera.width / TILE_SIZE / SCALER / 2);
    float startY = (camera.posY - camera.height / TILE_SIZE / SCALER / 2);
    float endX = startX + camera.width / TILE_SIZE / SCALER;
    float endY = startY + camera.height / TILE_SIZE / SCALER;

    //std::cout << "startX: " << startX << ", startY: " << startY << ", endX: " << endX << ", endY: " << endY << std::endl;

    // Render visible tiles
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            int tileX = (int)x;
            int tileY = (int)y;
            if (tileX >= 0 && tileX < WORLD_WIDTH && tileY >= 0 && tileY < WORLD_HEIGHT && world[tileX][tileY].isVisible) {
                drawTile(world[tileX][tileY], x - startX, y - startY);
            }
        }
    }

    // Render the player
    drawTile(player.playerTile, (camera.width / TILE_SIZE / SCALER / 2), camera.height / TILE_SIZE / SCALER / 2);
    //drawPlayer(player);
    //drawPlayer(player, camera.width / TILE_SIZE / SCALER / 2, camera.height / TILE_SIZE / SCALER / 2);

    // Render the selector
    drawTile(player.SelectorTile, player.SelectorX / TILE_SIZE / SCALER, player.SelectorY / TILE_SIZE / SCALER);

    glfwSwapBuffers(window);
}
