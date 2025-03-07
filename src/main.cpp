/*  
 * Sandboxia2D - A sandbox game written in C++ using OpenGL.
 * Copyright (c) 2024-2025 Benjamin Helle
 *  
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, either version 3 of the License, or  
 * (at your option) any later version.  
 *  
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  
 * GNU General Public License for more details.  
 *  
 * You should have received a copy of the GNU General Public License  
 * along with this program. If not, see <https://www.gnu.org/licenses/>.  
 */
#include <iostream>
#include "../include/main.h"
#include "../include/mod_api.h"
#include "../include/Sandboxia/renderer.h"
#include "../include/Sandboxia/player.h"
#include "../include/Sandboxia/ui.h"
#include "../include/Sandboxia/input.h"
#include "../include/Sandboxia/world.h"
#include <fstream>
#include <iomanip>

using namespace std;
using namespace ImGui;

ofstream logFile("log.txt"); // Log file


// Set default values
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

bool VSYNC = true;
bool COLLISION = true;
bool DEBUG = false;
bool MODS_ENABLED = true;

GLFWwindow* window;

Renderer renderer;
Player player;
Camera camera;
World world;

int initGame(){
    camera.posX = 0.0f;
    camera.posY = 0.0f;
    camera.width = SCREEN_WIDTH;
    camera.height = SCREEN_HEIGHT;

    player.posX = 1.0f;
    player.posY = WORLD_HEIGHT - 70.0f;
    player.PlayerSpeed = 10.0f;
    player.playerTile = DefaultTiles[TypePlayer];

    player.SelectorX = 0.0f;
    player.SelectorY = 0.0f;
    player.SelectedTileType = T_Grass;
    player.SelectorTile = DefaultTiles[TypeSelector];

    if (!glfwInit()) {
        log("[ERROR] Failed to initialize GLFW with error code: " + to_string(glfwGetError(NULL)));
        return -1;
    }

    // If using legacy renderer(2.1), change major to 2 and minor to 1

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    log("[INFO] Window size: " + to_string(SCREEN_WIDTH) + 'x' + to_string(SCREEN_HEIGHT));

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandboxia2D", NULL, NULL);
    if (!window) {
        log("[ERROR] Failed to create window with error code: " + to_string(glfwGetError(NULL)));
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log("[ERROR] Failed to initialize GLAD");
        return -1;
    }  

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scrollCallback);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    log("[INFO] Renderer device: " + string((const char*)glGetString(GL_RENDERER)));

    glClearColor(0.222f, 0.608f, 0.924f, 1.0f); // Background color
    glfwSwapBuffers(window);

    renderer.init();

    generateWorld(world, -1);

    glfwSwapInterval(VSYNC); // Enable/disable vsync

    InitUI(window);

    // Must load mods after other initialization.
    if (!MODS_ENABLED) return 0;
    LoadMods();
    return 0;
}

void log(string msg) {
    logFile << fixed << setprecision(2) << "[" << glfwGetTime() << "] " << msg << endl;
    cout << fixed << setprecision(2) << " [" << glfwGetTime()  << "] " << msg << endl;
}

string handleConsoleCommand(std::string command) {
    log("[INFO] Executing command: " + command);
    istringstream iss(command);
    string cmd;
    iss >> cmd;
    if (cmd == "/seed") {
        return "Current world seed: " + to_string(world.seed);
    } else if (cmd == "/debug") {
        DEBUG = !DEBUG;
        log("[INFO] DEBUG set to " + to_string(DEBUG));
        return "DEBUG set to " + to_string(DEBUG);
    } else if (sscanf(command.c_str(), "/tp %e %e", &player.posX, &player.posY) == 2) {
        return "Teleported to (" + to_string(player.posX) + ", " + to_string(player.posY) + ")";
    }
    return "Unknown command";
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    player.SelectorX = xpos;
    player.SelectorY = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    renderer.updateViewport(width, height);
    camera.height = height;
    camera.width = width;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

int main(int argc, char *argv[]) {
    world.width = WORLD_WIDTH; // Set default world size
    world.height = WORLD_HEIGHT;

    strcpy(world.name, "Test world");
    string args;
    for (int i = 0; i < argc; i++) {
        args += argv[i];
        if (i < argc - 1) {
            args += " ";
        }
    }
    log("--- Sandboxia2D " + string(VERSION) + " ---");
    log("[INFO] Launching with arguments: " + args);

    // Parse arguments
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
        } else if (arg == "-W" && i + 1 < argc) { // world
            char *worldPath = argv[++i];
            loadWorld(worldPath, &world);
        } else if (arg == "-disableMods") { // Disable mods
            MODS_ENABLED = false;
        } 
    }

    // Initialize the game
    initGame();

    float lastFrame, currentFrame, deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window)) { // Main game loop
        glClear(GL_COLOR_BUFFER_BIT);
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
 
        if (MODS_ENABLED) UpdateMods(deltaTime); 

        const auto& io = GetIO();

        if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) { // If is typing or using UI prevent player movement
            InputHandler(window, player, camera, world, deltaTime);
        }

        player.updatePlayer(player, world, deltaTime);

        InputHandlerUI(window, player, camera, world, deltaTime); // Always check for input for UI and other stuff

        renderer.RenderViewport(camera, player, world, window);

        HandleUI(window, world, player, renderer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown
    ExitUI();
    renderer.exit();
    glfwDestroyWindow(window);
    log("[INFO] Program shutting down");
    glfwTerminate();
    // Deallocate memory
    for (int i = 0; i < world.width; i++) {
        delete[] world.tiles[i];
    }
    delete[] world.tiles;
    logFile.close();
    return 0;
}

