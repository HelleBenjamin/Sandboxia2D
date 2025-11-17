#include "../include/Sandboxia/player.h"

float const GRAVITY = 20.0f;
float const JUMP_POWER = 2.0f;

static bool checkCollision(float x, float y, World& world) {
    int tileX = static_cast<int>(x);
    int tileY = static_cast<int>(y);
    if (tileX < 0 || tileX >= world.width || tileY < 0 || tileY >= world.height) {
        return true; // Return 1 if out of bounds
    }

    return world.tiles[tileX][tileY].isSolid; // Return 1 if solid
}

void Player::updatePlayer(Player &player, World& world, float deltaTime) {
    if (!player.onGround) {
        player.accelY = +GRAVITY; // Apply gravity
    } else {
        player.accelY = 0;
    }

    player.velX += player.accelX * deltaTime; // Add because the y-axis is inverted
    player.velY -= player.accelY * deltaTime;

    player.posX += player.velX * deltaTime;
    player.posY -= player.velY * deltaTime;

    if (player.posY >= WORLD_HEIGHT || checkCollision(player.posX, player.posY+1, world)) {
        player.velY = 0;
        player.onGround = true;
    } else {
        player.onGround = false;
    }
}

void Player::jump(Player &player) {
    player.velY = +JUMP_POWER;
    player.onGround = false;
}

void Player::move(int dx, int dy, float deltaTime, World& world, Player& player) {
    // Here's some code that controls the player

    float moveAmountX = dx * PlayerSpeed * deltaTime;
    float moveAmountY = dy * PlayerSpeed * deltaTime;

    float newX = posX + moveAmountX;
    float newY = posY + moveAmountY;

	if (dx > 0) { // Set player direction
		player.isMovingRight = true;
		player.isMovingLeft = false;
		player.isIdle = false;
	}
	else if (dx < 0) {
		player.isMovingLeft = true;
		player.isMovingRight = false;
		player.isIdle = false;
	}
	else {
		player.isIdle = true;
		player.isMovingRight = false;
		player.isMovingLeft = false;
	}

    if (!COLLISION) { // Skip if collision is disabled
        posX = newX;
        posY = newY;
        return;
    } 
    if (!checkCollision(newX, posY, world)) { // Basic collision checking, may not be the best but it works
        posX = newX;
    }
    if (!checkCollision(posX, newY, world)) {
        posY = newY;
    }
}
