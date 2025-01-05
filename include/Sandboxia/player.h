#ifndef PLAYER_H
#define PLAYER_H
#include "main.h"


class Player{
public:
    float posX, posY;
    float PlayerSpeed;

    double SelectorX, SelectorY;
    Tile SelectorTile; 

    Tile playerTile; // Used for rendering

    void move(int dx, int dy, float deltaTime, Tile world[][WORLD_HEIGHT]);
};

class Camera{
public:
    int posX, posY;
    int width, height; // viewport

    void updateCamera(Camera& camera, Player player);
};


#endif