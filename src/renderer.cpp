#include "../include/Sandboxia/renderer.h"
#include "../include/main.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

/* 
    OpenGL 3.0 Core based renderer
    Modern and fast compared to the legacy renderer.
    Use this as default.
*/

// Vertex and fragment shaders
const char* vertexShaderSource = R"glsl(
#version 130
in vec2 aPos;
in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 130
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture2D(texture1, TexCoord);
}
)glsl";

GLFWwindow* getCurrentWindow() {
    return glfwGetCurrentContext();
}

// Do some complicated opengl stuff that I don't understand :)
unsigned int Renderer::compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        log(LOG_ERR,"[RENDERER] ERROR::SHADER::COMPILATION_FAILED\n%s", infoLog);
    }
    return shader;
}

unsigned int Renderer::createShaderProgram() {
    log(LOG_INFO,"[RENDERER] Creating shader program");
    log(LOG_INFO,"[RENDERER] Compiling shaders");
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    log(LOG_INFO,"[RENDERER] Compiled shaders");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        log(LOG_ERR, "[RENDERER] ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
    }

    if(success) log(LOG_INFO,"[RENDERER] Created shader program successfully");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void Renderer::init() {
    // I do understand this :)
    log(LOG_INFO,"[RENDERER] Using 'Modern' as the render engine");
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

GLuint loadTexture(const char* filepath) {
    if (!filepath) log(LOG_ERR,"[RENDERER] Texture path is null");
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    if (!data) {
        log(LOG_ERR, "[RENDERER] Failed to load texture: %s", filepath);
        return(2);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    log(LOG_INFO, "[RENDERER] Loaded texture '%s' with ID %d", filepath, textureID);
    return textureID;
}

void unloadTexture(GLuint textureID) {
    glDeleteTextures(1, &textureID);
    log(LOG_INFO, "[RENDERER] Unloaded texture with ID %d", textureID);
}

void Renderer::loadTextures(){
    const char* path = "assets/textures/"; // Texture directory
    const char* filenames[] = {"player.png", "player_left.png", "player_right.png", "selector.png", "air.png", "grass.png", "stone.png", "dirt.png", "sand.png", "wood.png", "leaves.png"}; // The order must be the same as defined in world.h

    textures.resize(tileCount); // Allocate memory
    glGenTextures(tileCount, textures.data());

    for (int i = 0; i < tileCount; i++) {
        //textures.push_back(loadTexture(filenames[i]));
        char* filepath = (char*)malloc(strlen(path) + strlen(filenames[i]) + 1);
        strcpy(filepath, path);
        strcat(filepath, filenames[i]);
        textures[i] = loadTexture(filepath);
        free(filepath);
    }
}

void Renderer::freeTextures(){
    for (int i = 0; i < textures.size(); i++) {
        unloadTexture(textures[i]);
    }
}

void Renderer::exit() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    freeTextures();
}

void Renderer::drawTile(Tile tile, int x, int y) { // Draw the tile to x,y position
    glEnable(GL_BLEND); // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, textures[tile.type]);

    mat4 model = mat4(1.0f);
    model = translate(model, vec3(x * TILE_SIZE * SCALER, y * TILE_SIZE * SCALER, 0.0f));
    model = scale(model, vec3(TILE_SIZE * SCALER, TILE_SIZE * SCALER, 1.0f));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Renderer::drawTilef(Tile tile, float x, float y) { // Draw the tile to x,y position
    glEnable(GL_BLEND); // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, textures[tile.type]);

    mat4 model = mat4(1.0f);
    model = translate(model, vec3(x * TILE_SIZE * SCALER, y * TILE_SIZE * SCALER, 0.0f));
    model = scale(model, vec3(TILE_SIZE * SCALER, TILE_SIZE * SCALER, 1.0f));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Renderer::updateViewport(int width, int height) { // Update the viewport, fixes the streching bug
    glViewport(0, 0, width, height);

    mat4 projection = ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void Renderer::RenderViewport(Camera& camera, Player& player, World& world, GLFWwindow* window) {
    //glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Fix a bug where the player is inside of a tile

    // Deadzone and following camera. Way better than centered camera
    float deadzoneWidth = static_cast<float>(camera.width) / TILE_SIZE / SCALER * 0.25f;
    float deadzoneHeight = static_cast<float>(camera.height) / TILE_SIZE / SCALER * 0.25f;
    float smoothness = 0.15f; // Smaller = more smooth, bigger = snapper
    
    // Calculate player position relative to camera center
    float playerRelX = player.posX - camera.posX;
    float playerRelY = player.posY - camera.posY;
    
    // Calculate target camera position based on deadzone
    float targetX = camera.posX;
    float targetY = camera.posY;
    
    // Horizontal deadzone check
    if (playerRelX > deadzoneWidth) {
        targetX = player.posX - deadzoneWidth;
    } else if (playerRelX < -deadzoneWidth) {
        targetX = player.posX + deadzoneWidth;
    }
    
    // Vertical deadzone check
    if (playerRelY > deadzoneHeight) {
        targetY = player.posY - deadzoneHeight;
    } else if (playerRelY < -deadzoneHeight) {
        targetY = player.posY + deadzoneHeight;
    }
    
    // Smoothly interpolate camera to target position
    camera.posX += (targetX - camera.posX) * smoothness;
    camera.posY += (targetY - camera.posY) * smoothness;

    // Calculate the visible tiles
    // Use rounding to fix clipping, maybe even use int instead of float?
    float startX = floor(camera.posX - static_cast<float>(camera.width) / TILE_SIZE / SCALER / 2);
    float startY = floor(camera.posY - static_cast<float>(camera.height) / TILE_SIZE / SCALER / 2);
    float endX = ceil(startX + static_cast<float>(camera.width) / TILE_SIZE / SCALER);
    float endY = ceil(startY + static_cast<float>(camera.height) / TILE_SIZE / SCALER);

    // Render visible tiles
    for (int x = startX; x < endX; ++x) {
        for (int y = startY; y < endY; ++y) {
            int tileX = static_cast<int>(x); // Instead of (int)x
            int tileY = static_cast<int>(y);
            if (tileX >= 0 && tileX < world.width && tileY >= 0 && tileY < world.height && world.tiles[tileX][tileY].type != T_Air) {
                drawTile(world.tiles[tileX][tileY], x - startX, y - startY);
            }
        }
    }

    // Render the player at their actual world position
    float playerScreenX = player.posX - startX;
    float playerScreenY = player.posY - startY;
    
	if (player.isMovingRight) drawTile(DefaultTiles[TypePlayer_Right], playerScreenX, playerScreenY);
	else if (player.isMovingLeft) drawTile(DefaultTiles[TypePlayer_Left], playerScreenX, playerScreenY);
	else drawTile(player.playerTile, playerScreenX, playerScreenY);

    // Render the selector
    drawTile(player.SelectorTile, (player.SelectorX / TILE_SIZE / SCALER), (player.SelectorY / TILE_SIZE / SCALER));

    //glfwSwapBuffers(window);
}
