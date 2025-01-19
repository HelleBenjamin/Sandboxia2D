#include "../include/mod_api.h"

ModAPI* gameAPI;

extern "C" {
    void ModInitialize(ModAPI* api) {
        gameAPI = api;
        if(01 > gameAPI->GetAPIVersion()) {
            gameAPI->DebugLog("[MOD] This mod requires a newer version of Sandboxia2D Mod API. Some features may not work. Current version: " + std::to_string(gameAPI->GetAPIVersion()));
        }
        gameAPI->DebugLog("[MOD] Hello from testmod!");
        int ModTileType = gameAPI->GetTileTypeCount();
        GLuint PTexture = gameAPI->LoadTexture("assets/player.png");
        gameAPI->AddNewTile(PTexture, ModTileType, true);
    }

    void ModUpdate(float deltaTime) {
    }
}