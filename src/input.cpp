#include "../include/Sandboxia/input.h"

// Keyboard and UI input
void InputHandlerUI(SDL_Window* window, SDL_Event* event, Player& player, Camera& camera, World& world, float deltaTime) {
    static bool pressed = false; // Prevent spamming the input
    static auto& io = ImGui::GetIO();
    // UI and other input
    if (!pressed && !io.WantCaptureMouse) {
        SDL_assert(event->type == SDL_EVENT_KEY_DOWN);
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
            isMenuOpen = !isMenuOpen;
            pressed = true;
        } else if (event->key.scancode == SDL_SCANCODE_T) { 
            isConsoleOpen = !isConsoleOpen;
            pressed = true;
        } else if (event->key.scancode == SDL_SCANCODE_F1) {
            saveWorld("world", &world);
            pressed = true;
        } 
        
        // Debug only
        else if (event->key.scancode == SDL_SCANCODE_P && DEBUG) {
            log("[INFO] Player position: (" + std::to_string(player.posX) + ", " + std::to_string(player.posY) + ")");
            pressed = true;
        } else if (event->key.scancode == SDL_SCANCODE_O && DEBUG) {
            log("[INFO] Selector position: (" + std::to_string((player.SelectorX / TILE_SIZE / SCALER) + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)) +  ", " + std::to_string((player.SelectorY / TILE_SIZE / SCALER) + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)) + ")");
            pressed = true;
        } else if (event->key.scancode == SDL_SCANCODE_F && DEBUG) {
            log("[INFO] Current FPS: " + std::to_string(1.0f / deltaTime));
            pressed = true;
        }

    } else {
        pressed = false;
    }
}

// Player movement
void InputHandler(SDL_Window* window, SDL_Event* event,Player& player, Camera& camera, World& world, float deltaTime) {
    int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
    int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset
    SDL_assert(event->type == SDL_EVENT_KEY_DOWN);
    // Tile selection & player movement, reserved tiles 0-6 (may change when new tiles are added)
    if (event->key.scancode == SDL_SCANCODE_1) { // Grass
        player.SelectedTileType = TypeGrass;
    } else if (event->key.scancode == SDL_SCANCODE_2) { // Stone
        player.SelectedTileType = TypeStone;
    } else if (event->key.scancode == SDL_SCANCODE_3) { // Dirt
        player.SelectedTileType = TypeDirt;
    } else if (event->key.scancode == SDL_SCANCODE_4) { // Sand
        player.SelectedTileType = TypeSand;
    } else if (event->key.scancode == SDL_SCANCODE_5) { // Wood
        player.SelectedTileType = TypeWood;
    }

    // Modded tiles
    else if (event->key.scancode == SDL_SCANCODE_6) {
        player.SelectedTileType = 7;
    }

    if (event->key.scancode == SDL_SCANCODE_W) player.move(0, -1, deltaTime, world);
    if (event->key.scancode == SDL_SCANCODE_S) player.move(0, 1, deltaTime, world);
    if (event->key.scancode == SDL_SCANCODE_A) player.move(-1, 0, deltaTime, world);
    if (event->key.scancode == SDL_SCANCODE_D) player.move(1, 0, deltaTime, world);
    if (event->key.scancode == SDL_SCANCODE_SPACE) player.jump(player);


}

void MouseEvent(SDL_Window* window, SDL_Event* event,Player& player, Camera& camera, World& world, float deltaTime) {
    int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
    int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset
    // Place and destroy
    SDL_assert(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN);
    if (event->button.button == SDL_BUTTON_LEFT) { // destroy tile
        if (SelX < 0 || SelX >= world.width || SelY < 0 || SelY >= world.height) log("[ERROR] Tried to destroy tile out of bounds at " + std::to_string(SelX) + ", " + std::to_string(SelY));
        else{
            world.tiles[SelX][SelY].type = T_Air;
            world.tiles[SelX][SelY].isSolid = false;
        };
    } else if (event->button.button == SDL_BUTTON_RIGHT) { // place tile
        if (SelX < 0 || SelX >= world.width || SelY < 0 || SelY >= world.height) log("[ERROR] Tried to place tile out of bounds at " + std::to_string(SelX) + ", " + std::to_string(SelY));
        else if (world.tiles[SelX][SelY].type == T_Air){ // Only place if tile is air
            world.tiles[SelX][SelY].type = player.SelectedTileType;
            world.tiles[SelX][SelY].isSolid = true;
        };
    }    
}

/*void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    if (yoffset > 0) {
        player.SelectedTileType = (TileType)((player.SelectedTileType - 1 + tileCount) % tileCount);
        if (player.SelectedTileType <= TypeAir) player.SelectedTileType = TypeGrass;
    } else if (yoffset < 0) {
        player.SelectedTileType = (TileType)((player.SelectedTileType + 1) % tileCount);
        if (player.SelectedTileType <= TypeAir) player.SelectedTileType = TypeGrass;
    }
}*/
