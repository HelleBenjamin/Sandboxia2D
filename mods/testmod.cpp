#include "../include/mod_api.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

ModAPI* gameAPI;

extern "C" {
    EXPORT void ModInitialize(ModAPI* api) {
        gameAPI = api;
        if(01 > gameAPI->GetAPIVersion()) {
            gameAPI->DebugLog("[MOD] This mod requires a newer version of Sandboxia2D Mod API. Some features may not work. Current version: " + std::to_string(gameAPI->GetAPIVersion()));
        }
        gameAPI->DebugLog("[MOD] Hello from testmod!");
        int ModTileType = gameAPI->GetTileTypeCount();
        GLuint PTexture = gameAPI->LoadTexture("assets/player.png");
        gameAPI->AddNewTile(PTexture, ModTileType, true);
    }

    EXPORT void ModUpdate(float deltaTime) {
		int ModTileType = gameAPI->GetTileTypeCount();
		if (gameAPI->IsKeyPressed(GLFW_KEY_P)) {
			gameAPI->SetTile(gameAPI->GetPlayerPos().x, gameAPI->GetPlayerPos().y, ModTileType);
		}
    }
}