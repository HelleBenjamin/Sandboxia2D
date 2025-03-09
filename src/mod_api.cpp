#include <iostream>
#include <vector>
#include <filesystem>

#include "../include/mod_api.h"

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIBRARY(lib) LoadLibrary(lib)
    #define GET_PROC_ADDRESS GetProcAddress
    #define CLOSE_LIBRARY FreeLibrary
    #define LIBRARY_EXTENSION ".dll"
#else
    #include <dlfcn.h>
    #define LOAD_LIBRARY(lib) dlopen(lib, RTLD_LAZY)
    #define GET_PROC_ADDRESS dlsym
    #define CLOSE_LIBRARY dlclose
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

void AddNewTile(SDL_Texture* texture, int type, bool isSolid) {
    DefaultTiles[type] = {type, 0, isSolid, 0};
    tileCount++;
    renderer.textures.push_back(texture);
}

SDL_Texture* LoadTexture(const char* filepath) {
    return renderer.loadTexture(filepath);
}

void FreeTexture(SDL_Texture* texture) {
    renderer.unloadTexture(texture);
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
ModAPI api = { &GetPlayerPos, &SetPlayerPos, &AddNewTile, LoadTexture, &FreeTexture, &SetTile, &GetTile, &GetCurrTileType, &GetTileTypeCount, &GetWorldSeed, &GetAPIVersion, &GetGameVersion, &DebugLog };
vector<string> loadedMods;

void LoadMods() {
    string modfp = "mods/";

    for (const auto& entry : fs::directory_iterator(modfp)) {
        string modPath = entry.path().string();

        #ifdef _WIN32
            HMODULE handle = LOAD_LIBRARY(modPath.c_str());
        #else
            void* handle = LOAD_LIBRARY(modPath.c_str());
        #endif
        
        if (entry.path().extension() == LIBRARY_EXTENSION) {
            #ifdef _WIN32
                auto ModInitialize = (InitMod) GET_PROC_ADDRESS((HMODULE)handle, "ModInitialize");
                auto ModUpdate = (UpdateMod) GET_PROC_ADDRESS((HMODULE)handle, "ModUpdate");
            #else
                auto ModInitialize = (InitMod) GET_PROC_ADDRESS(handle, "ModInitialize");
                auto ModUpdate = (UpdateMod) GET_PROC_ADDRESS(handle, "ModUpdate");
            #endif

            if (!handle) {
                log("[ERROR] Failed to load mod: " + modPath);
                continue;
            }

            if (!ModInitialize || !ModUpdate) { 
                log("[ERROR] Failed to load mod: " + modPath);
                #ifdef _WIN32
                    CLOSE_LIBRARY((HMODULE)handle);
                #else
                    CLOSE_LIBRARY(handle);
                #endif
                continue;
            }

            loadedMods.push_back(modPath);
            mods.push_back({handle, ModInitialize, ModUpdate});
            
            ModInitialize(&api);
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

