#include <iostream>
#include "../include/main.h"
#include "../include/Sandboxia/renderer.h"
#include "../include/Sandboxia/player.h"
#include <fstream>
#include <cmath>
#include <iomanip>

/*  Sandboxia2D by Benjamin Helle (C) 2024-2025

    This is the main file for the Sandboxia2D project.
    It contains the main function and the game loop.
    Other functions are defined in other files.

*/

using namespace std;

ofstream logFile("log.txt"); // Log file

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
Tile world[WORLD_WIDTH][WORLD_HEIGHT] = {0};

bool VSYNC = true;
bool COLLISION = true;
bool DEBUG = false;

Renderer renderer;
Player player;
Camera camera;


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
                world[x][y].type = 3; // Dirt
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

void saveWorld(Tile world[][WORLD_HEIGHT], std::string filename) {
    /* Save format:
        Save into a text file with the following format:
        [Tile with data] [Another tile with data] ...

        Tile with data: type isVisible tileState isSolid tileID
    
    */
    std::ofstream file(filename + ".txt");
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            file << "[ " << world[x][y].type << " " << world[x][y].isVisible << " " << world[x][y].tileState << " " << world[x][y].isSolid << " " << world[x][y].tileID << " ]";
        }
        file << endl;
    }
    file.close();
    log("[INFO] Saved world to " + filename);
}

void loadWorld(Tile (&world)[WORLD_WIDTH][WORLD_HEIGHT], std::string filename) { // TODO
}

void InputHandler(GLFWwindow* window, Player& player, Tile world[][WORLD_HEIGHT], float deltaTime) {
    int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
    int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.move(0, -1, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.move(0, 1, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.move(-1, 0, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.move(1, 0, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) saveWorld(world, "world"); // Does work
    //if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) loadWorld(world, "world"); // Currently doesn't work

    // Set placable tile type
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { // Grass
        player.SelectedTileType = T_Grass;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { // Stone
        player.SelectedTileType = T_Stone;
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { // Dirt
        player.SelectedTileType = T_Dirt;
    }


    // Debug features
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && DEBUG) log("[INFO] Player position: (" + to_string(player.posX) + ", " + to_string(player.posY) + ")");
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && DEBUG) log("[INFO] Selector position: (" + to_string((player.SelectorX / TILE_SIZE / SCALER) + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)) +  ", " + to_string((player.SelectorY / TILE_SIZE / SCALER) + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)) + ")");
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && DEBUG) log("[INFO] Current Tile at cursor: " + to_string(world[SelX][SelY].type) + " " + to_string(world[SelX][SelY].isVisible) + " " + to_string(world[SelX][SelY].isSolid));
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && DEBUG) log("[INFO] Current FPS: " + to_string(1.0f / deltaTime));

    // Place and destroy
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // destroy tile
        if (SelX < 0 || SelX >= WORLD_WIDTH || SelY < 0 || SelY >= WORLD_HEIGHT) log("[ERROR] Tried to destroy tile out of bounds at " + to_string(SelX) + ", " + to_string(SelY));
        else{
            world[SelX][SelY].type = 0;
            world[SelX][SelY].isVisible = false;
            world[SelX][SelY].isSolid = false;
        };
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // place tile
        if (SelX < 0 || SelX >= WORLD_WIDTH || SelY < 0 || SelY >= WORLD_HEIGHT) log("[ERROR] Tried to place tile out of bounds at " + to_string(SelX) + ", " + to_string(SelY));
        else if (world[SelX][SelY].type == 0){ // Only place if tile is air
            world[SelX][SelY].type = player.SelectedTileType;
            world[SelX][SelY].isVisible = true;
            world[SelX][SelY].isSolid = true;
        };
    }
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
        } else if (arg == "-d") { // debug
            DEBUG = 1;
            log("[INFO] DEBUG set to " + to_string(DEBUG));
        }
    }

    camera.posX = 0.0f;
    camera.posY = 0.0f;
    camera.width = SCREEN_WIDTH;
    camera.height = SCREEN_HEIGHT;

    player.posX = 1.0f;
    player.posY = WORLD_HEIGHT - 70.0f;
    player.PlayerSpeed = 10.0f;
    player.playerTile.type = T_Player;
    player.playerTile.isVisible = true;
    player.playerTile.isSolid = false;

    player.SelectorX = 0.0f;
    player.SelectorY = 0.0f;
    player.SelectedTileType = T_Grass;
    player.SelectorTile.type = T_Selector;
    player.SelectorTile.isVisible = true;
    player.SelectorTile.isSolid = false;

    if (!glfwInit()) {
        log("[ERROR] Failed to initialize GLFW");
        return -1;
    }

    // If using legacy renderer(2.1), change major to 2 and minor to 1

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


    log("[INFO] Window size: " + to_string(SCREEN_WIDTH) + 'x' + to_string(SCREEN_HEIGHT));

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandboxia2D", NULL, NULL);
    if (!window) {
        log("[ERROR] Failed to create window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    log("[INFO] Renderer device: " + string((const char*)glGetString(GL_RENDERER)));

    // TODO: Make game initialization function

    glClearColor(0.222f, 0.608f, 0.924f, 1.0f); // Background color
    glfwSwapBuffers(window);


    renderer.init();

    generateWorld(world);

    glfwSwapInterval(VSYNC); // Enable/disable vsync

    float lastFrame, currentFrame, deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window)) { // Main game loop
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        InputHandler(window, player, world, deltaTime);

        renderer.RenderViewport(camera, player, world, window);
    }

    log("[INFO] Preparing to shutdown");

    renderer.exit();
    glfwDestroyWindow(window);
    glfwTerminate();
    log("[INFO] Program shutting down");
    logFile.close();
    return 0;
}