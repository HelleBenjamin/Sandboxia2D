#include "../include/Sandboxia/player.h"

bool checkCollision(int x, int y, World& world) {
    if (x < 0 || x >= world.width || y < 0 || y >= world.height) {
        return true; // Return 1 if out of bounds
    }

    return world.tiles[x][y].isSolid; // Return 1 if solid
}


void Camera::updateCamera(Camera& camera, Player player) { // Update camera to follow player
    camera.posX = player.posX / 2;
    camera.posY = player.posY / 2;
}

void Player::move(int dx, int dy, float deltaTime, World& world) {
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
