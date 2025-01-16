#include <iostream>
#include "../include/main.h"
#include "../include/Sandboxia/renderer.h"
#include "../include/Sandboxia/player.h"
#include "../include/Sandboxia/ui.h"
#include <fstream>
#include <cmath>
#include <iomanip>
#include <cstring>

/*  Sandboxia2D by Benjamin Helle (C) 2024-2025

    This is the main file for the Sandboxia2D project.
    It contains the main function and the game loop.
    Other functions are defined in other files.

*/

using namespace std;
using namespace ImGui;

ofstream logFile("log.txt"); // Log file

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

bool VSYNC = true;
bool COLLISION = true;
bool DEBUG = false;

Renderer renderer;
Player player;
Camera camera;
World world;

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
    }
    return "Unknown command";
}

void generateWorld(World& world) {
    if (world.seed == 0) {
        srand(time(NULL));
        world.seed = rand();
    } else srand(world.seed);


    int terrainHeight[WORLD_WIDTH];
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        double noise = sin((double)x / 16.0 + world.seed) * 8.0 + cos((double)x / 32.0 + world.seed) * 4.0; // Combine waves for variation
        terrainHeight[x] = (WORLD_HEIGHT / 2) + static_cast<int>(noise);
    }

    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            int terrainY = WORLD_HEIGHT - 1 - y;
            if (terrainY > terrainHeight[x]) {
                world.tiles[x][y].type = T_Air; // Air
                world.tiles[x][y].isSolid = false;
            } else if (terrainY == terrainHeight[x]) {
                world.tiles[x][y].type = T_Grass; // Grass
                world.tiles[x][y].isSolid = true;
            } else if (terrainY > terrainHeight[x] - 4) {
                world.tiles[x][y].type = T_Dirt; // Dirt
                world.tiles[x][y].isSolid = true;
            } else {
                world.tiles[x][y].type = T_Stone; // Stone
                world.tiles[x][y].isSolid = true;
            }
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    player.SelectorX = xpos;
    player.SelectorY = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // TODO: Fix the streching
    glViewport(0, 0, width, height);
    camera.width = width;
    camera.height = height;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void loadWorld(const char* filePath, World* world) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        log("[ERROR] Failed to open file for loading");
        return;
    }
    fread(world, sizeof(World), 1, file); // Read the entire World structure
    log("[INFO] Loaded world");
    log("[INFO] World name: " + string(world->name));
    log("[INFO] World size: " + to_string(world->width) + "x" + to_string(world->height));
    log("[INFO] World seed: " + to_string(world->seed));
    fclose(file);
}

void saveWorld(const char* filePath, const World* world) {
    FILE* file = fopen(filePath, "wb");
    if (!file) {
        log("[ERROR] Failed to open file for saving");
        return;
    }
    fwrite(world, sizeof(World), 1, file); // Write the entire World structure
    log("[INFO] Saved world");
    fclose(file);
}

// Keyboard and UI input
void InputHandlerUI(GLFWwindow* window, Player& player, World& world, float deltaTime) {
    static bool pressed = false; // Prevent spamming the input

    // UI and other input
    if (!pressed) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            isMenuOpen = !isMenuOpen;
            pressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { 
            isConsoleOpen = !isConsoleOpen;
            pressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
            saveWorld("world", &world);
            pressed = true;
        } 
        
        // Debug only
        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && DEBUG) {
            log("[INFO] Player position: (" + to_string(player.posX) + ", " + to_string(player.posY) + ")");
            pressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && DEBUG) {
            log("[INFO] Selector position: (" + to_string((player.SelectorX / TILE_SIZE / SCALER) + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)) +  ", " + to_string((player.SelectorY / TILE_SIZE / SCALER) + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)) + ")");
            pressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && DEBUG) {
            log("[INFO] Current FPS: " + to_string(1.0f / deltaTime));
            pressed = true;
        }

    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
               glfwGetKey(window, GLFW_KEY_T) != GLFW_PRESS && 
               glfwGetKey(window, GLFW_KEY_F1) != GLFW_PRESS && 
               glfwGetKey(window, GLFW_KEY_P) != GLFW_PRESS && 
               glfwGetKey(window, GLFW_KEY_O) != GLFW_PRESS && 
               glfwGetKey(window, GLFW_KEY_F) != GLFW_PRESS) {
        pressed = false;
    }
}

// Player movement
void InputHandler(GLFWwindow* window, Player& player, World& world, float deltaTime) {
    int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
    int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset

     // Tile selection & player movement
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { // Grass
        player.SelectedTileType = T_Grass;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { // Stone
        player.SelectedTileType = T_Stone;
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { // Dirt
        player.SelectedTileType = T_Dirt;
    } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { // Sand
        player.SelectedTileType = T_Sand;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.move(0, -1, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.move(0, 1, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.move(-1, 0, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.move(1, 0, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player.jump(player);

    // Place and destroy
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // destroy tile
        if (SelX < 0 || SelX >= world.width || SelY < 0 || SelY >= world.height) log("[ERROR] Tried to destroy tile out of bounds at " + to_string(SelX) + ", " + to_string(SelY));
        else{
            world.tiles[SelX][SelY].type = T_Air;
            world.tiles[SelX][SelY].isSolid = false;
        };
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // place tile
        if (SelX < 0 || SelX >= world.width || SelY < 0 || SelY >= world.height) log("[ERROR] Tried to place tile out of bounds at " + to_string(SelX) + ", " + to_string(SelY));
        else if (world.tiles[SelX][SelY].type == 0){ // Only place if tile is air
            world.tiles[SelX][SelY].type = player.SelectedTileType;
            world.tiles[SelX][SelY].isSolid = true;
        };
    }
}


int main(int argc, char *argv[]) {

    world.width = WORLD_WIDTH;
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
    player.playerTile.isSolid = false;

    player.SelectorX = 0.0f;
    player.SelectorY = 0.0f;
    player.SelectedTileType = T_Grass;
    player.SelectorTile.type = T_Selector;
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
        log("[ERROR] Failed to initialize GLAD");
        return -1;
    }  

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    log("[INFO] Renderer device: " + string((const char*)glGetString(GL_RENDERER)));

    // TODO: Make game initialization function, invert the y-axis

    glClearColor(0.222f, 0.608f, 0.924f, 1.0f); // Background color
    glfwSwapBuffers(window);

    renderer.init();

    generateWorld(world);

    glfwSwapInterval(VSYNC); // Enable/disable vsync

    float lastFrame, currentFrame, deltaTime = 0.0f;

    InitUI(window);

    while (!glfwWindowShouldClose(window)) { // Main game loop
        glClear(GL_COLOR_BUFFER_BIT);
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        const auto& io = ImGui::GetIO();

        if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) { // If is typing or using UI prevent player movement
            InputHandler(window, player, world, deltaTime);
        }

        player.updatePlayer(player, world, deltaTime);

        InputHandlerUI(window, player, world, deltaTime); // Always check for input for UI and other stuff

        renderer.RenderViewport(camera, player, world, window);

        HandleUI(window, world, player, renderer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ExitUI();
    renderer.exit();
    glfwDestroyWindow(window);
    log("[INFO] Program shutting down");
    glfwTerminate();
    logFile.close();
    return 0;
}