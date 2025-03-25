#ifndef MOD_API_H
#define MOD_API_H

#include "../include/main.h"
#include "../include/Sandboxia/world.h"
#include "../include/Sandboxia/player.h"
#include "../include/Sandboxia/renderer.h"
#include "../include/Sandboxia/ui.h"
#include "../include/Sandboxia/input.h"

#define API_VERSION_STR "0.1"
#define API_VERSION_NUM 01

/* Sandboxia2D Mod API v0.1

    This is a simple API for mods to add new features to Sandboxia2D.
    For deeper modding, modify the source code directly.



    How to make a mod:

    TODO

    You should always include this line of code at the top of your mod file:
    if(TargetVersion > API_VERSION_NUM) cout << "This mod requires a newer version of Sandboxia2D Mod API. Some features may not work.\n";
    

*/

extern "C" {
    struct vec2 { float x, y; };

    // Define functions
    typedef vec2 (*GetPlayerPosFunc)();
    typedef void (*SetPlayerPosFunc)(vec2 pos);
    typedef void (*AddNewTileFunc)(GLuint texture, int type, bool isSolid);
    typedef GLuint (*LoadTextureFunc)(const char* path);
    typedef void (*FreeTextureFunc)(GLuint texture);
    typedef void (*SetTileFunc)(int x, int y, int type);
    typedef Tile (*GetTileFunc)(int x, int y);
    typedef int (*GetCurrTileTypeFunc)();
    typedef int (*GetTileTypeCountFunc)();
    typedef int (*GetWorldSeedFunc)();

    // Input/Output
    typedef bool (*IsKeyDownFunc)(int key);
    typedef bool (*IsKeyPressedFunc)(int key);

    typedef int (*GetAPIVersionFunc)();
    typedef int (*GetGameVersionFunc)();
    typedef void (*DebugLogFunc)(std::string text);

    // API structure
    struct ModAPI {
        GetPlayerPosFunc GetPlayerPos;
        SetPlayerPosFunc SetPlayerPos;
        AddNewTileFunc AddNewTile;
        LoadTextureFunc LoadTexture;
        FreeTextureFunc FreeTexture;
        SetTileFunc SetTile;
        GetTileFunc GetTile;
        GetCurrTileTypeFunc GetCurrTileType;
        GetTileTypeCountFunc GetTileTypeCount;
        GetWorldSeedFunc GetWorldSeed;

        IsKeyDownFunc IsKeyDown;
        IsKeyPressedFunc IsKeyPressed;

        GetAPIVersionFunc GetAPIVersion;
        GetGameVersionFunc GetGameVersion;
        DebugLogFunc DebugLog;
    };

    // Required mod functions
    typedef void (*InitMod)(ModAPI* api);
    typedef void (*UpdateMod)(float deltaTime);

}

void LoadMods();
void UpdateMods(float deltaTime);
const std::vector<std::string>& GetLoadedMods();

#endif