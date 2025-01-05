#include <iostream>
#include "../include/main.h"
#include "../include/Sandboxia/renderer.h"
#include "../include/Sandboxia/player.h"
#include <fstream>
#include <iomanip>

/*  Sandboxia2D by Benjamin Helle (C) 2024-2025

    This is the main file for the Sandboxia2D project.
    It contains the main function and the game loop.
    Other functions are defined in other files.

*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

ofstream logFile("log.txt"); // Log file

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
Tile world[WORLD_WIDTH][WORLD_HEIGHT] = {0};

bool VSYNC = true;
bool COLLISION = true;

Renderer *renderer = new Renderer();
Player player;
Camera camera;

/* TILE TYPES:

    0: Air
    1: Grass
    2: Stone
    3: Player
    4: Dirt
    5: Selector

*/


GLuint loadTexture(const char* filepath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (!data) {
        log("[ERROR] Failed to load texture: " + string(filepath));
        return -1;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    log("[INFO] Loaded texture: " + string(filepath) + " with ID: " + to_string(textureID));
    return textureID;
}

void unloadTexture(GLuint textureID) {
    glDeleteTextures(1, &textureID);
    log("[INFO] Unloaded texture with ID: " + to_string(textureID));
}

void log(string msg) {
    logFile << fixed << setprecision(2) << "[" << glfwGetTime() << "] " << msg << endl;
    cout << fixed << setprecision(2) << " [" << glfwGetTime()  << "] " << msg << endl;
}

void generateWorld(Tile world[][WORLD_HEIGHT]) {
    srand(time(nullptr));

    int terrainHeight[WORLD_WIDTH];
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        double noise = sin((double)x / 16.0) * 8.0 + cos((double)x / 32.0) * 4.0; // Combine waves for variation
        terrainHeight[x] = (WORLD_HEIGHT / 2) + (int)(noise);
    }

    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            int terrainY = WORLD_HEIGHT - 1 - y;
            if (terrainY > terrainHeight[x]) {
                world[x][y].type = 0; // Air
                world[x][y].isVisible = false;
                world[x][y].isSolid = false;
            } else if (terrainY == terrainHeight[x]) {
                world[x][y].type = 1; // Grass
                world[x][y].isVisible = true;
                world[x][y].isSolid = true;
            } else if (terrainY > terrainHeight[x] - 4) {
                world[x][y].type = 4; // Dirt
                world[x][y].isVisible = true;
                world[x][y].isSolid = true;
            } else {
                world[x][y].type = 2; // Stone
                world[x][y].isVisible = true;
                world[x][y].isSolid = true;
            }
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    player.SelectorX = xpos;
    player.SelectorY = ypos;
}


int main(int argc, char *argv[]) {
    std::string args;
    for (int i = 0; i < argc; i++) {
        args += argv[i];
        if (i < argc - 1) {
            args += " ";
        }
    }
    log("--- Sandboxia2D " + string(VERSION) + " ---");
    log("[INFO] Launching with arguments: " + args);
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-w" && i + 1 < argc) { // w for width
            SCREEN_WIDTH = atoi(argv[++i]);
        } else if (arg == "-h" && i + 1 < argc) { // h for height
            SCREEN_HEIGHT = atoi(argv[++i]);
        } else if (arg == "-v" && i + 1 < argc) { // vsync
            VSYNC = (argc > i + 1) ? atoi(argv[++i]) : 1;
            log("[INFO] VSYNC set to " + to_string(VSYNC));
        } else if (arg == "-c" && i + 1 < argc) { // collision
            COLLISION = (argc > i + 1) ? atoi(argv[++i]) : 1;
            log("[INFO] COLLISION set to " + to_string(COLLISION));
        }
    }


    camera.posX = 0.0f;
    camera.posY = 0.0f;
    camera.width = SCREEN_WIDTH;
    camera.height = SCREEN_HEIGHT;

    player.posX = 1.0f;
    player.posY = WORLD_HEIGHT - 70.0f;
    player.PlayerSpeed = 10.0f;
    player.playerTile.type = 3;
    player.playerTile.isVisible = true;
    player.playerTile.isSolid = false;

    player.SelectorX = 0.0f;
    player.SelectorY = 0.0f;
    player.SelectorTile.type = 5;
    player.SelectorTile.isVisible = true;
    player.SelectorTile.isSolid = false;

    if (!glfwInit()) {
        log("[ERROR] Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


    log("[INFO] Window size: " + to_string(SCREEN_WIDTH) + 'x' + to_string(SCREEN_HEIGHT));

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandboxia2D", NULL, NULL);
    if (!window) {
        log("[ERROR] Failed to create window");
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    glfwMakeContextCurrent(window);

    glClearColor(0.222f, 0.608f, 0.924f, 1.0f); // Background color
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer->loadTextures();

    generateWorld(world);

    glfwSwapInterval(VSYNC); // Enable vsync

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) { // Main game loop
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
        int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.move(0, -1, deltaTime, world);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.move(0, 1, deltaTime, world);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.move(-1, 0, deltaTime, world);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.move(1, 0, deltaTime, world);
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) log("[INFO] Player position: (" + to_string(player.posX) + ", " + to_string(player.posY) + ")");
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) log("[INFO] Selector position: (" + to_string((player.SelectorX / TILE_SIZE / SCALER) + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)) +  ", " + to_string((player.SelectorY / TILE_SIZE / SCALER) + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)) + ")");
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) log("[INFO] Current Tile at cursor: " + to_string(world[SelX][SelY].type) + " " + to_string(world[SelX][SelY].isVisible) + " " + to_string(world[SelX][SelY].isSolid));
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // destroy block
            if (SelX < 0 || SelX >= WORLD_WIDTH || SelY < 0 || SelY >= WORLD_HEIGHT) log("[ERROR] Tried to destroy block out of bounds at " + to_string(SelX) + ", " + to_string(SelY));
            else{
                world[SelX][SelY].type = 0;
                world[SelX][SelY].isVisible = false;
                world[SelX][SelY].isSolid = false;
            };
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // place block
            if (SelX < 0 || SelX >= WORLD_WIDTH || SelY < 0 || SelY >= WORLD_HEIGHT) log("[ERROR] Tried to place block out of bounds at " + to_string(SelX) + ", " + to_string(SelY));
            else {
            world[SelX][SelY].type = 2;
            world[SelX][SelY].isVisible = true;
            world[SelX][SelY].isSolid = true;
            };
        }

        //cout << "Player position: " << player.posX << ", " << player.posY << endl;
        //cout << "Camera position: " << camera.posX << ", " << camera.posY << endl;
        //cout << "Current FPS: " << 1.0f / deltaTime << endl;
        renderer->RenderViewport(camera, player, world, window);
        glfwPollEvents();
    }
    renderer->freeTextures();
    log("[INFO] Unloaded all textures");
    delete renderer;
    log("[INFO] Deallocated renderer");
    glfwDestroyWindow(window);
    glfwTerminate();
    log("[INFO] Program shutting down");
    logFile.close();
    return 0;
}