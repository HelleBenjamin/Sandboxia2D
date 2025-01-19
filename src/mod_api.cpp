#include <iostream>
#include <vector>
#include <filesystem>

#include "../include/mod_api.h"

#ifdef _WIN32 // Definitions for OS specific functions
    #include <windows.h>
    #define LOAD_LIBRARY(lib) LoadLibrary(lib)
    #define GET_PROC_ADDRESS GetProcAddress
    #define CLOSE_LIBRARY FreeLibrary
#else
    #include <dlfcn.h>
    #define LOAD_LIBRARY(lib) dlopen(lib, RTLD_LAZY)
    #define GET_PROC_ADDRESS dlsym
    #define CLOSE_LIBRARY dlclose
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
ModAPI api = { &GetPlayerPos, &SetPlayerPos, &AddNewTile, LoadTexture, &FreeTexture, &SetTile, &GetTile, &GetCurrTileType, &GetTileTypeCount,  &GetAPIVersion, &GetGameVersion, &DebugLog };
vector<string> loadedMods;

void* LoadMod(const char* modPath) {
    void* handle = LOAD_LIBRARY(modPath);
    if (!handle) {
        printf("Failed to load mod: %s\n", modPath);
    }
    return handle;
}

void LoadMods() {
    string modfp = "mods/"; // Change if using a different directory

    for (const auto& entry : filesystem::directory_iterator(modfp)) {
        string modPath = entry.path().string();
        
        if (entry.path().extension() == ".so" || entry.path().extension() == ".dll") { // .so for linux, .dll for windows
            void* handle = LOAD_LIBRARY(modPath.c_str());
            if (!handle) {
                log("[ERROR] Failed to load mod: " + modPath);
                continue;
            }

            void* hModule = handle;

            auto ModInitialize = (InitMod) GET_PROC_ADDRESS(hModule, "ModInitialize");
            auto ModUpdate = (UpdateMod) GET_PROC_ADDRESS(hModule, "ModUpdate");

            if (!ModInitialize || !ModUpdate) { 
                log("[ERROR] Invalid mod structure: " + modPath);
                CLOSE_LIBRARY(hModule);
                continue;
            }

            loadedMods.push_back(modPath);
            mods.push_back({hModule, ModInitialize, ModUpdate});
            
            ModInitialize(&api); // Give access to API
            log("[INFO] Loaded mod: " + modPath);
        }
    }
}

void UpdateMods(float deltaTime) {
    for (auto& mod : mods) {
        mod.ModUpdate(deltaTime);
    }
}

const std::vector<std::string>& GetLoadedMods() {
    return loadedMods;
}