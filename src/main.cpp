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

/*  Sandboxia2D by Benjamin Helle (C) 2024-2025

    This is the main file for the Sandboxia2D project.
    It contains the main function and the game loop.
    Other functions are defined in other files.

*/

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
bool SHOULD_EXIT = false;

SDL_Window* window = NULL;
SDL_Event event;

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

    player.SelectorX = 1.0f;
    player.SelectorY = 1.0f;
    player.SelectedTileType = T_Grass;
    player.SelectorTile = DefaultTiles[TypeSelector];


    log("[INFO] Window size: " + to_string(SCREEN_WIDTH) + 'x' + to_string(SCREEN_HEIGHT));

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        log("[ERROR] SDL Video init failed");
        log("[ERROR] SDL error: " + string(SDL_GetError()));
        return -1;
    }
    if (!SDL_Init(SDL_INIT_EVENTS)) {
        log("[ERROR] SDL Event init failed");
        log("[ERROR] SDL error: " + string(SDL_GetError()));
        return -1;  
    }

    window = SDL_CreateWindow("Sandboxia2D", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS);

    if (!window) {
        log("[ERROR] Failed to create window");
        log("[ERROR] SDL error: " + string(SDL_GetError()));
        SDL_Quit();
        return -1;
    }
 
    //log("[INFO] Renderer device: " + string((const char*)glGetString(GL_RENDERER)));

    renderer.initr(window);

    generateWorld(world, -1);

    SDL_SetRenderVSync(renderer.renderer, VSYNC); // Enable/disable vsync

    InitUI(window);

    log("[INFO] Initialized game");

    // Must load mods after other initialization.
    if (!MODS_ENABLED) return 0;
    LoadMods();
    return 0;
}

void log(string msg) {
    logFile << "[" << SDL_GetTicks() << "] " << msg << endl;
    cout << " [" << SDL_GetTicks()  << "] " << msg << endl;
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

/*void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    player.SelectorX = xpos;
    player.SelectorY = ypos;
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

    while (!SHOULD_EXIT) { // Main game loop
        currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        SDL_PollEvent(&event);

        if (MODS_ENABLED) UpdateMods(deltaTime); 

        const auto& io = GetIO();

        if (event.type == SDL_EVENT_QUIT) {
            SHOULD_EXIT = true;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            log("[INFO] Key pressed");
            //if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) { // If is typing or using UI prevent player movement
                InputHandler(window, &event, player, camera, world, deltaTime);
            //}
            InputHandlerUI(window, &event, player, camera, world, deltaTime); // Always check for input for UI and other stuff
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            MouseEvent(window, &event, player, camera, world, deltaTime);
        }
        float cursorX, cursorY;
        SDL_GetMouseState(&cursorX, &cursorY);
        player.SelectorX = cursorX;
        player.SelectorY = cursorY;

        player.updatePlayer(player, world, deltaTime);

        renderer.RenderViewport(camera, player, world, window);

        HandleUI(window, world, player, renderer);

    }

    // Shutdown
    ExitUI();
    renderer.exitr();
    SDL_DestroyWindow(window);
    log("[INFO] Program shutting down");
    // Deallocate memory
    for (int i = 0; i < world.width; i++) {
        delete[] world.tiles[i];
    }
    delete[] world.tiles;
    logFile.close();
    SDL_Quit();
    return 0;
}

