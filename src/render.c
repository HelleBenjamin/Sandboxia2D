#include "../include/render.h"
#include "../include/main.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>

Texture2D textures[0xFF]; /* Maybe make this dynamic*/
uint8_t num_textures = 0;
 
int render_init() {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandboxia2D");
  SetTargetFPS(TARGET_FPS);
  DisableCursor();

  const char* filenames[] = {"player.png", "player_left.png", "player_right.png", "selector.png", "air.png", "grass.png", "stone.png", "dirt.png", "sand.png", "wood.png", "leaves.png"}; // The order must be the same as defined in world.h

  for (int i = 0; i < sizeof(filenames)/sizeof(filenames[0]); i++) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%sassets/textures/%s", gamepath, filenames[i]);
    textures[i] = LoadTexture(filepath);
    num_textures++;
  }

  return 0;
}

int render_exit() {
  for (int i = 0; i < num_textures; i++) {
    UnloadTexture(textures[i]);
  }
  CloseWindow();
  return 0;
}

/* Translate player direction to texture index*/
uint8_t player_dir[4] = {
  0, 0, 1, 2
};

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
      TileConfig config = tile_configs[world->tiles[translate_index(tileX, tileY)].type];
      Vector2 position = (Vector2){(float)(x*TILE_SIZE*RENDER_SCALE), (float)(y*TILE_SIZE*RENDER_SCALE)};
      if ((tileX >= 0) && (tileX < WORLD_WIDTH) && (tileY >= 0) && (tileY < WORLD_HEIGHT) && (config.is_visible)) {
        DrawTextureEx(textures[config.texture_id], position, 0.0f, RENDER_SCALE, WHITE);
      }
    }
  }

  /* Draw player */
  DrawTextureEx(textures[player_dir[player->player.direction]], player->player.position, 0.0f, RENDER_SCALE, WHITE);

  /* Draw entities */
  for (int i = 0; i < num_entities; i++) {
    DrawTextureEx(textures[entities[i].texture_id], entities[i].position, 0.0f, RENDER_SCALE, WHITE);
  }
  

  /* Draw selector */
  if (selX >= 0 && selX < WORLD_WIDTH && selY >= 0 && selY < WORLD_HEIGHT) {
    Vector2 selectorWorldPos = (Vector2){(float)selX * TILE_SIZE * RENDER_SCALE,(float)selY * TILE_SIZE * RENDER_SCALE};
    DrawTextureEx(textures[TypeSelector_Texture], selectorWorldPos, 0.0f, RENDER_SCALE, WHITE);
  }

  EndMode2D();
  DrawFPS(10, 10);

  /* Draw coordinates and selected tile*/
  char buffer[32];
  Vector2 actual_coords = (Vector2){player->player.position.x / TILE_SIZE / RENDER_SCALE, player->player.position.y / TILE_SIZE / RENDER_SCALE};
  sprintf(buffer, "X: %d Y: %d", (int)actual_coords.x, (int)actual_coords.y);
  DrawText(buffer, 10, 30, 20, BLACK);

  sprintf(buffer, "Selected tile: %s", tile_configs[player->selected_tile].name);
  DrawText(buffer, 10, 50, 20, BLACK);

  EndDrawing();
}