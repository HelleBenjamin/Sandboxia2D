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
#include "../include/Sandboxia/renderer.h"
#include "../include/Sandboxia/player.h"
#include "../include/Sandboxia/input.h"
#include "../include/Sandboxia/world.h"
#include <fstream>
#include <iomanip>
#include <raylib.h>
#include <cstring>
#include <stdarg.h>

using namespace std;


ofstream logFile("log.txt"); // Log file


// Set default values
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

bool VSYNC = true;
bool COLLISION = true;
bool DEBUG = false;
bool MODS_ENABLED = true;
bool SOUNDS_ENABLED = true;
int LOG_LEVEL = 4; // Log all messages
bool WORLD_LOADED = false;

Renderer renderer;
Player player;
Camera2D camera;
World world;

int initGame(){
    srand(time(NULL));

    player.posX = (float)(rand() % (WORLD_WIDTH - 2) + 1);
    player.posY = WORLD_HEIGHT - 70.0f;
    if (player.posY > WORLD_HEIGHT) player.posY = 70.0f;
    player.PlayerSpeed = 10.0f;
    player.playerTile = DefaultTiles[TypePlayer];

    player.SelectorX = 0.0f;
    player.SelectorY = 0.0f;
    player.SelectedTileType = T_Grass;
    player.SelectorTile = DefaultTiles[TypeSelector];

    renderer.init();

    if (!WORLD_LOADED) generateWorld(world, -1); // Generate a new world

    // Must load mods after other initialization.
    if (!MODS_ENABLED) return 0;
    //LoadMods();
    return 0;
}

void log(int type, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    string msg = buffer;
    double time = GetFrameTime();
    string types[] = {"[NULL] ","[ERROR] ", "[WARNING] ", "[INFO] ", "[DEBUG] "};
    string log_type = types[type];
    cout << fixed << setprecision(2) << " [" << time  << "] " << log_type << msg << endl;
    if (type <= LOG_LEVEL) logFile << fixed << setprecision(2) << "[" << time << "] " << log_type << msg << endl;
}

string handleConsoleCommand(std::string command) {
    // Handle console commands, there might be a better way to do this, but this works
    log(SLOG_INFO, "Executing command: ", command.c_str());
    string cmd = command;
    if (cmd == "/seed") {
        return "Current world seed: " + to_string(world.seed);
    } else if (cmd == "/debug") {
        DEBUG = !DEBUG;
        log(SLOG_INFO,"DEBUG set to ", DEBUG);
        return "DEBUG set to " + to_string(DEBUG);
    } else if (sscanf(command.c_str(), "/tp %e %e", &player.posX, &player.posY) == 2) {
        return "Teleported to (" + to_string(player.posX) + ", " + to_string(player.posY) + ")";
    }
    return "Unknown command";
}

/*void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Get current cursor position
    player.SelectorX = xpos;
    player.SelectorY = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Update viewport and window size
    renderer.updateViewport(width, height);
    camera.height = height;
    camera.width = width;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}*/

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
    
    log(SLOG_INFO,"---Sandboxia2D %s---", VERSION_STR);
    log(SLOG_INFO,"Launching with arguments: %s", args.c_str());

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-w" && i + 1 < argc) { // w for width
            SCREEN_WIDTH = atoi(argv[++i]);
        } else if (arg == "-h" && i + 1 < argc) { // h for height
            SCREEN_HEIGHT = atoi(argv[++i]);
        } else if (arg == "-v" && i + 1 < argc) { // vsync
            VSYNC = (argc > i + 1) ? atoi(argv[++i]) : 1;
            log(SLOG_INFO, "VSYNC set to %d", VSYNC);
        } else if (arg == "-c" && i + 1 < argc) { // collision
            COLLISION = (argc > i + 1) ? atoi(argv[++i]) : 1;
            log(SLOG_INFO, "COLLISION set to %d", COLLISION);
        } else if (arg == "-debug") { // debug
            DEBUG = 1;
            log(SLOG_INFO, "DEBUG set to %d", DEBUG);
        } else if (arg == "-world" && i + 1 < argc) { // world
            WORLD_LOADED = true;
            char *worldPath = argv[++i];
            if(loadWorld(worldPath, &world)) WORLD_LOADED = false; // If unsuccessful, create a new world
        } else if (arg == "-noMods") { // Disable mods
            MODS_ENABLED = false;
        } else if (arg == "-noSounds") { // Disable sounds
            SOUNDS_ENABLED = false;
        } else if (arg == "-log" && i + 1 < argc) { // log level
            LOG_LEVEL = atoi(argv[++i]);
        }
    }

    // Initialize the game
    initGame();

    log(SLOG_INFO,"Game initialized");

    float lastFrame{}, currentFrame, deltaTime = 0.0f;

    while (!WindowShouldClose()) { // Main game loop
        currentFrame = GetFrameTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
 
        //if (MODS_ENABLED) UpdateMods(deltaTime); // Update all mods

        player.updatePlayer(player, world, deltaTime); // Update the player

        renderer.RenderViewport(player, world); // Render the viewport

    }

    // Shutdown
    renderer.exit();

    log(SLOG_INFO,"Program shutting down");

    // Deallocate memory
    for (int i = 0; i < world.width; i++) {
        delete[] world.tiles[i];
    }
    delete[] world.tiles;
    logFile.close();
    return 0;
}
