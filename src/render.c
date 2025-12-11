#include "../include/render.h"
#include "../include/main.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Texture2D textures[0xFF]; /* Maybe make this dynamic*/
uint8_t allocated_textures = 0;
 
int render_init() {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandboxia2D");
  SetTargetFPS(TARGET_FPS);
  DisableCursor();

  const char* path = "assets/textures/"; // Texture directory
  const char* filenames[] = {"player.png", "player_left.png", "player_right.png", "selector.png", "air.png", "grass.png", "stone.png", "dirt.png", "sand.png", "wood.png", "leaves.png"}; // The order must be the same as defined in world.h

  for (int i = 0; i < sizeof(filenames)/sizeof(filenames[0]); i++) {
    char* filepath = (char*)malloc(strlen(path) + strlen(filenames[i]) + 1);
    strcpy(filepath, path);
    strcat(filepath, filenames[i]);
    textures[i] = LoadTexture(filepath);
    allocated_textures++;
    free(filepath);
  }

  return 0;
}

int render_exit() {
  for (int i = 0; i < allocated_textures; i++) {
    UnloadTexture(textures[i]);
  }
  CloseWindow();
  return 0;
}

void render_game(Camera2D* camera, Player* player, World* world){
  BeginDrawing();
  ClearBackground(SKYBLUE);

  BeginMode2D(*camera);

  Vector2 worldPos = GetScreenToWorld2D(player->selector, *camera);
  int selX = (int)(worldPos.x / (TILE_SIZE * RENDER_SCALE));
  int selY = (int)(worldPos.y / (TILE_SIZE * RENDER_SCALE));

  // Use rounding to fix clipping, maybe even use int instead of float?
  float startX = ((camera->target.x - camera->offset.x / camera->zoom) / TILE_SIZE / RENDER_SCALE);
  float startY = ((camera->target.y - camera->offset.y / camera->zoom) / TILE_SIZE / RENDER_SCALE);
  float endX = (startX + (float)SCREEN_WIDTH / camera->zoom / TILE_SIZE / RENDER_SCALE);
  float endY = (startY + (float)SCREEN_HEIGHT / camera->zoom / TILE_SIZE / RENDER_SCALE);

  // Render visible tiles
  for (int x = startX; x < endX; ++x) {
    for (int y = startY; y < endY; ++y) {
      int tileX = (int)(x);
      int tileY = (int)(y);
      Tile tile = world->tiles[translate_index(tileX, tileY)];
      Vector2 position = (Vector2){(float)(x*TILE_SIZE*RENDER_SCALE), (float)(y*TILE_SIZE*RENDER_SCALE)};
      if ((tileX >= 0) && (tileX < WORLD_WIDTH) && (tileY >= 0) && (tileY < WORLD_HEIGHT) && (tile.type != TypeAir)) {
        //drawTile(world.tiles[tileX][tileY], x - startX, y - startY);
        DrawTextureEx(textures[tile.type], position, 0.0f, RENDER_SCALE, WHITE);
      }
    }
  }

  DrawTextureEx(textures[TypeStone], (Vector2){0, 0}, 0.0f, RENDER_SCALE, WHITE); /* Test tile, used as (0,0) reference*/

  DrawTextureEx(textures[player->direction], player->position, 0.0f, RENDER_SCALE, WHITE);

  /* Draw selector */
  if (selX >= 0 && selX < WORLD_WIDTH && selY >= 0 && selY < WORLD_HEIGHT) {
    Vector2 selectorWorldPos = (Vector2){(float)selX * TILE_SIZE * RENDER_SCALE,(float)selY * TILE_SIZE * RENDER_SCALE};
    DrawTextureEx(textures[TypeSelector], selectorWorldPos, 0.0f, RENDER_SCALE, WHITE);
    //DrawRectangleLines((int)selectorWorldPos.x, (int)selectorWorldPos.y, TILE_SIZE * RENDER_SCALE, TILE_SIZE * RENDER_SCALE, BLACK);
  }

  EndMode2D();

  //DrawRectangleLines(player->selector.x, player->selector.y, 40, 40, BLACK);

  DrawFPS(10, 10);

  char buffer[32];
  Vector2 actual_coords = (Vector2){player->position.x / TILE_SIZE / RENDER_SCALE, player->position.y / TILE_SIZE / RENDER_SCALE};
  sprintf(buffer, "X: %d Y: %d", (int)actual_coords.x, (int)actual_coords.y);
  DrawText(buffer, 10, 30, 20, BLACK);

  sprintf(buffer, "Selected tileID: %d", player->selected_tile);
  DrawText(buffer, 10, 50, 20, BLACK);

  EndDrawing();
}