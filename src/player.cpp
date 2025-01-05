#include "../include/Sandboxia/player.h"
#include <cmath>

bool checkCollision(float newX, float newY, Tile world[][WORLD_HEIGHT]) {
    int left = static_cast<int>(newX);
    int right = static_cast<int>(newX + 1); // Player width = 1 tile
    int top = static_cast<int>(newY);
    int bottom = static_cast<int>(newY + 1); // Player height = 1 tile

    // Check if player is out of bounds
    if (left < 0 || right >= WORLD_WIDTH || top < 0 || bottom >= WORLD_HEIGHT) {
        return true;
    }

    if ((left >= 0 && left < WORLD_WIDTH && top >= 0 && top < WORLD_HEIGHT && world[left][top].isSolid) ||
        (right >= 0 && right < WORLD_WIDTH && top >= 0 && top < WORLD_HEIGHT && world[right][top].isSolid) ||
        (left >= 0 && left < WORLD_WIDTH && bottom >= 0 && bottom < WORLD_HEIGHT && world[left][bottom].isSolid) ||
        (right >= 0 && right < WORLD_WIDTH && bottom >= 0 && bottom < WORLD_HEIGHT && world[right][bottom].isSolid)) {
        return true;
    }
    return false;
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

    if (!checkCollision(newX-1, posY-1, world) && COLLISION) {
        posX = newX;
    }
    if (!checkCollision(posX-1, newY-1, world) && COLLISION) {
        posY = newY;
    }
    if (!COLLISION) { // Skip if collision is disabled
        posX = newX;
        posY = newY;
    } 
}
