#include "../include/Sandboxia/player.h"

bool checkCollision(int x, int y, Tile world[][WORLD_HEIGHT]) {
    if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) {
        return true; // Return 1 if out of bounds
    }

    return world[x][y].isSolid; // Return 1 if solid
}


void Camera::updateCamera(Camera& camera, Player player) { // Update camera to follow player
    camera.posX = player.posX / 2;
    camera.posY = player.posY / 2;
}

void Player::move(int dx, int dy, float deltaTime, Tile world[][WORLD_HEIGHT]) {
    float moveAmountX = dx * PlayerSpeed * deltaTime;
    float moveAmountY = dy * PlayerSpeed * deltaTime;

    float newX = posX + moveAmountX;
    float newY = posY + moveAmountY;

    if (!COLLISION) { // Skip if collision is disabled
        posX = newX;
        posY = newY;
        return;
    } 
    if (!checkCollision(newX, posY, world)) {
        posX = newX;
    }
    if (!checkCollision(posX, newY, world)) {
        posY = newY;
    }
}
