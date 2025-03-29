#include <iostream>
#include <vector>
#include <filesystem>

#include "../include/mod_api.h"

#ifdef _WIN32 // Windows
    #include <windows.h>
    #define LOAD_LIBRARY(lib) LoadLibrary(lib)
    #define GET_PROC_ADDRESS GetProcAddress
    #define CLOSE_LIBRARY FreeLibrary
    #define MOD_PATH "mods\\"
    #define LIBRARY_EXTENSION ".dll"
#else // Linux
    #include <dlfcn.h>
    #define LOAD_LIBRARY(lib) dlopen(lib, RTLD_LAZY)
    #define GET_PROC_ADDRESS dlsym
    #define CLOSE_LIBRARY dlclose
    #define MOD_PATH "mods/"
    #define LIBRARY_EXTENSION ".so"
#endif


namespace fs = std::filesystem;
using namespace std;

// API Calls
vec2 GetPlayerPos() {
    return { player.posX, player.posY };
}

void SetPlayerPos(vec2 pos) {
    player.posX = pos.x;
    player.posY = pos.y;
}

void AddNewTile(GLuint texture, int type, bool isSolid) {
    DefaultTiles[type] = {type, 0, isSolid, 0};
    tileCount++;
    renderer.textures.push_back(texture);
}

GLuint LoadTexture(const char* filepath) {
    return loadTexture(filepath);
}

void FreeTexture(GLuint texture) {
    unloadTexture(texture);
}

void SetTile(int x, int y, int type) {
    world.tiles[x][y] = DefaultTiles[type];
}

Tile GetTile(int x, int y) {
    return world.tiles[x][y];
}

int GetCurrTileType() {
    return player.SelectedTileType;
}

int GetTileTypeCount() {
    return tileCount;
}

int GetWorldSeed() {
    return world.seed;
}

// Input/Output
bool IsKeyDown(int key) {
    GLFWwindow* window = getCurrentWindow();
    if (window == NULL) return false;
    if (glfwGetKey(window, key) == GLFW_KEY_DOWN) return true;
    return false;
}

bool IsKeyPressed(int key) {
    GLFWwindow* window = getCurrentWindow();
    if (window == NULL) return false;
    if (glfwGetKey(window, key) == GLFW_PRESS) return true;
    return false;
}


// API Info
int GetAPIVersion() {
    return API_VERSION_NUM;
}

int GetGameVersion() {
    return SANDBOXIA_VERSION_NUM;
}

void DebugLog(string text) {
    log(text);
}

struct Mod {
    void* handle;
    InitMod ModInit;
    UpdateMod ModUpdate;
};

vector<Mod> mods; // All mods are stored here
ModAPI api = { &GetPlayerPos, &SetPlayerPos, &AddNewTile, LoadTexture, &FreeTexture, &SetTile, &GetTile, &GetCurrTileType, &GetTileTypeCount, &GetWorldSeed, &IsKeyDown, &IsKeyPressed, &GetAPIVersion, &GetGameVersion, &DebugLog };
vector<string> loadedMods;

void LoadMods() {
    string modfp = MOD_PATH;

	if (!fs::exists(modfp)) { // Check if mods folder exists, this prevents crashes
		log("[ERROR] Mods folder not found");
		MODS_ENABLED = false;
		return;
	}

    for (const auto& entry : fs::directory_iterator(modfp)) {
        if (!entry.is_regular_file()) continue; // Skip directories
        
        string modPath = entry.path().string();
        string extension = entry.path().extension().string();

		if (extension != LIBRARY_EXTENSION) continue; // Skip non mod files

        #ifdef _WIN32
            HMODULE handle = LOAD_LIBRARY(modPath.c_str());
        #else
            void* handle = LOAD_LIBRARY(modPath.c_str());
        #endif

        if (!handle) {
			log("[ERROR] Failed to load mod: " + modPath); // Can be caused by different architecture
            continue;
        }

        auto ModInitialize = (InitMod) GET_PROC_ADDRESS(handle, "ModInitialize");
        auto ModUpdate = (UpdateMod) GET_PROC_ADDRESS(handle, "ModUpdate");

		if (!ModInitialize || !ModUpdate) {  // Check if the mod has the required functions
            log("[ERROR] Invalid mod: " + modPath);
            CLOSE_LIBRARY(handle);
            continue;
        }

        loadedMods.push_back(modPath);
        mods.push_back({handle, ModInitialize, ModUpdate});

        ModInitialize(&api);
        log("[INFO] Loaded mod: " + modPath);
    }
}

void UpdateMods(float deltaTime) { // Update all mods
    for (auto& mod : mods) {
        mod.ModUpdate(deltaTime);
    }
}

const std::vector<std::string>& GetLoadedMods() {
    return loadedMods;
}

