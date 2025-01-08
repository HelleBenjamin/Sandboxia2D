#include "../include/Sandboxia/renderer.h"
#include "../include/main.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace glm;

/* 
    OpenGL 3.0 Core based renderer
    Modern and fast compared to the legacy renderer.
    Use this as default.
*/


// Vertex and fragment shaders
const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}
)glsl";

unsigned int Renderer::compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        log("[RENDERER] ERROR::SHADER::COMPILATION_FAILED\n" + std::string(infoLog));
    }
    return shader;
}

unsigned int Renderer::createShaderProgram() {
    log("[RENDERER] Creating shader program");
    log("[RENDERER] Compiling shaders");
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    log("[RENDERER] Compiled shaders");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        log("[RENDERER] ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
    }

    if(success) log("[RENDERER] Created shader program successfully");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void Renderer::init() {
    log("[RENDERER] Using 'Modern' as the render engine");
    loadTextures();

    float vertices[] = { // Tile vertices
        // positions    // texture coords
        0.0f, 1.0f,    0.0f, 1.0f,
        1.0f, 1.0f,    1.0f, 1.0f,
        1.0f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f,    0.0f, 0.0f
    };

    unsigned int indices[] = { 
        0, 1, 2,
        2, 3, 0
    };

    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    shaderProgram = createShaderProgram();
    mat4 projection = ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    
}

void Renderer::loadTextures(){
    AirTex = loadTexture("assets/air.png");
    GrassTex = loadTexture("assets/grass.png");
    StoneTex = loadTexture("assets/stone.png");
    DirtTex = loadTexture("assets/dirt.png");
    PlayerTex = loadTexture("assets/player.png");
    SelectorTex = loadTexture("assets/selection.png");
}

void Renderer::freeTextures(){
    unloadTexture(AirTex);
    unloadTexture(GrassTex);
    unloadTexture(StoneTex);
    unloadTexture(PlayerTex);
    unloadTexture(DirtTex);
    unloadTexture(SelectorTex);
}

void Renderer::exit() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    freeTextures();
}

void Renderer::drawTile(Tile tile, int x, int y) {
    if (!tile.isVisible) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (tile.type) { // Bind right texture according to tile type, TODO: Replace with enum
        case 0:
            glBindTexture(GL_TEXTURE_2D, AirTex);
            break;
        case 1:
            glBindTexture(GL_TEXTURE_2D, GrassTex);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, StoneTex);
            break;
        case 3:
            glBindTexture(GL_TEXTURE_2D, PlayerTex);
            break;
        case 4:
            glBindTexture(GL_TEXTURE_2D, DirtTex);
            break;
        case 5:
            glBindTexture(GL_TEXTURE_2D, SelectorTex);
            break;
        default:
            log("[ERROR] Invalid tile texture type: " + std::to_string(tile.type));
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
    }

    mat4 model = mat4(1.0f);
    model = translate(model, vec3(x * TILE_SIZE * SCALER, y * TILE_SIZE * SCALER, 0.0f));
    model = scale(model, vec3(TILE_SIZE * SCALER, TILE_SIZE * SCALER, 1.0f));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::RenderViewport(Camera& camera, Player& player, Tile world[][WORLD_HEIGHT], GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);

    camera.posX += (player.posX - camera.posX);
    camera.posY += (player.posY - camera.posY);

    // Calculate the visible tiles
    float startX = (camera.posX - camera.width / TILE_SIZE / SCALER / 2);
    float startY = (camera.posY - camera.height / TILE_SIZE / SCALER / 2);
    float endX = startX + camera.width / TILE_SIZE / SCALER;
    float endY = startY + camera.height / TILE_SIZE / SCALER;

    // Render visible tiles
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            int tileX = (int)x;
            int tileY = (int)y;
            if (tileX >= 0 && tileX < WORLD_WIDTH && tileY >= 0 && tileY < WORLD_HEIGHT && world[tileX][tileY].isVisible) {
                drawTile(world[tileX][tileY], x - startX, y - startY);
            }
        }
    }

    // Render the player
    drawTile(player.playerTile, (camera.width / TILE_SIZE / SCALER / 2), camera.height / TILE_SIZE / SCALER / 2);

    // Render the selector
    drawTile(player.SelectorTile, player.SelectorX / TILE_SIZE / SCALER, player.SelectorY / TILE_SIZE / SCALER);

    glfwSwapBuffers(window);
}
