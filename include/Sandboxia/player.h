#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "../main.h"
#include "world.h"

extern const float GRAVITY;
extern float const JUMP_POWER;



class Player{
public:
    float posX, posY;
    float PlayerSpeed;

    float velX, velY; // velocity
    float accelX, accelY; // acceleration
    bool onGround;

    float SelectorX, SelectorY;
    Tile SelectorTile;

    unsigned int SelectedTileType;
    Tile playerTile; // Used for rendering

    void move(int dx, int dy, float deltaTime, World& world);
    void updatePlayer(Player &player, World& world, float deltaTime);
    void jump(Player &player);
};

class Camera{
public:
    int posX, posY;
    int width, height; // viewport

    void updateCamera(Camera& camera, Player player);
};

extern Player player;
extern Camera camera;

#endif