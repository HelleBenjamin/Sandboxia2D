#include "../include/Sandboxia/input.h"

// Keyboard and UI input
void InputHandlerUI(GLFWwindow* window, Player& player, Camera& camera, World& world, float deltaTime) {
    static bool pressed = false; // Prevent spamming the input
    static auto& io = ImGui::GetIO();
    // UI and other input
    if (!pressed && !io.WantCaptureMouse) {
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
            log("[INFO] Player position: (" + std::to_string(player.posX) + ", " + std::to_string(player.posY) + ")");
            pressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && DEBUG) {
            log("[INFO] Selector position: (" + std::to_string((player.SelectorX / TILE_SIZE / SCALER) + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)) +  ", " + std::to_string((player.SelectorY / TILE_SIZE / SCALER) + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)) + ")");
            pressed = true;
        } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && DEBUG) {
            log("[INFO] Current FPS: " + std::to_string(1.0f / deltaTime));
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
void InputHandler(GLFWwindow* window, Player& player, Camera& camera, World& world, float deltaTime) {
    int SelX = static_cast<int>((player.SelectorX / TILE_SIZE / SCALER)  + (camera.posX - camera.width / TILE_SIZE / SCALER / 2)); // Calculate x + offset
    int SelY = static_cast<int>((player.SelectorY / TILE_SIZE / SCALER)  + (camera.posY - camera.height / TILE_SIZE / SCALER / 2)); // Calculate y + offset

    // Tile selection & player movement, reserved tiles 0-6 (may change when new tiles are added)
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { // Grass
        player.SelectedTileType = TypeGrass;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { // Stone
        player.SelectedTileType = TypeStone;
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { // Dirt
        player.SelectedTileType = TypeDirt;
    } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { // Sand
        player.SelectedTileType = TypeSand;
    } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) { // Wood
        player.SelectedTileType = TypeWood;
    }

    // Modded tiles
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        player.SelectedTileType = 7;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.move(0, -1, deltaTime, world, player);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.move(0, 1, deltaTime, world, player);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.move(-1, 0, deltaTime, world, player);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.move(1, 0, deltaTime, world, player);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player.jump(player);

    // Place and destroy
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // destroy tile
        if (SelX < 0 || SelX >= world.width || SelY < 0 || SelY >= world.height) log("[ERROR] Tried to destroy tile out of bounds at " + std::to_string(SelX) + ", " + std::to_string(SelY));
        else{
            world.tiles[SelX][SelY].type = T_Air;
            world.tiles[SelX][SelY].isSolid = false;
        };
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // place tile
        if (SelX < 0 || SelX >= world.width || SelY < 0 || SelY >= world.height) log("[ERROR] Tried to place tile out of bounds at " + std::to_string(SelX) + ", " + std::to_string(SelY));
        else if (world.tiles[SelX][SelY].type == T_Air){ // Only place if tile is air
            world.tiles[SelX][SelY].type = player.SelectedTileType;
            world.tiles[SelX][SelY].isSolid = true;
        };
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    if (yoffset > 0) {
        player.SelectedTileType = (TileType)((player.SelectedTileType - 1 + tileCount) % tileCount);
        if (player.SelectedTileType <= TypeAir) player.SelectedTileType = TypeGrass;
    } else if (yoffset < 0) {
        player.SelectedTileType = (TileType)((player.SelectedTileType + 1) % tileCount);
        if (player.SelectedTileType <= TypeAir) player.SelectedTileType = TypeGrass;
    }
}
