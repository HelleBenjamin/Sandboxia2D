#include "../include/Sandboxia/ui.h"

using namespace ImGui;

bool isConsoleOpen = false;
bool isMenuOpen = false;

void InitUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO& io = GetIO(); (void)io;
    StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void HandleUI(GLFWwindow* window, World &world, Player &player, Renderer &renderer) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    if (isConsoleOpen) ConsoleUI();
    if (isMenuOpen) MenuUI(window, world);

    // Selected tile
    ImVec2 window_pos = GetIO().DisplaySize;
    SetNextWindowPos(ImVec2(window_pos.x / 2, window_pos.y - 20), ImGuiCond_Always, ImVec2(0.5f, 1.0f));
    Begin("Selected tile", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    Text("Selected tile:");
    Image((ImTextureID)renderer.textures[player.SelectedTileType], ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
    End();


    // Debug
    if (DEBUG) DebugUI();

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void ConsoleUI() {
    static char console_buf[256] = "";
    static char console_out[256] = "";
    SetNextWindowSize(ImVec2(500, 200), ImGuiCond_FirstUseEver);
    Begin("Console");
    InputText("##console", console_buf, IM_ARRAYSIZE(console_buf));
    if (Button("Exec") || (IsKeyPressed(ImGuiKey_Enter))) {
        std::string command = console_buf;
        memset(console_buf, 0, sizeof(console_buf));
        strcpy(console_out, handleConsoleCommand(command).c_str());
    }
    TextWrapped("%s", console_out);
    End();
}

void DebugUI() {
    Begin("Debug");
    Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / GetIO().Framerate, GetIO().Framerate); // Display FPS
    End();
}

void MenuUI(GLFWwindow* window ,World &world) {
    static char world_name[32] = "world";
    SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    Begin("Menu");
    Text("World name: ");
    InputText("##world_name", world_name, IM_ARRAYSIZE(world_name));
    if (Button("Load")) {
        loadWorld(world_name, &world);
    } else if (Button("Save")) {
        saveWorld(world_name, &world);
    } else if (Button("Exit")) {
        glfwSetWindowShouldClose(window, true);
    }
    End();
}

void ExitUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
}