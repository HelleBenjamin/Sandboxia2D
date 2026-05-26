#include "../include/player.h"
#include "../include/main.h"
#include "../include/collision.h"
#include <math.h>

void handle_input(Player *player, World *world, Camera2D *camera, float dt) {
  float playerWidth  = PLAYER_COLLISION_W;
  float playerHeight = PLAYER_COLLISION_H;

  player->selector = GetMousePosition();

  int new_selected_tile = (int)((float)(player->selected_tile) + roundf(GetMouseWheelMove()));
  if (new_selected_tile >= 0 && new_selected_tile <= num_tile_configs) player->selected_tile = new_selected_tile;

  Vector2 worldPos = GetScreenToWorld2D(player->selector, *camera); /* Simple conversion*/
  int selX = (int)(worldPos.x / (TILE_SIZE * RENDER_SCALE)); /* Use FULL_TILE_SIZE? */
  int selY = (int)(worldPos.y / (TILE_SIZE * RENDER_SCALE));
  
  Vector2 new_position = player->player.position;
  
  /* Player movement */
  if (IsKeyDown(KEY_W)) {
    new_position.y -= player->player.speed * dt;
    player->player.direction = DIR_UP;
  }
  if (IsKeyDown(KEY_S)) {
    new_position.y += player->player.speed * dt;
    player->player.direction = DIR_DOWN;
  }
  if (IsKeyDown(KEY_A)) {
    new_position.x -= player->player.speed * dt;
    player->player.direction = DIR_LEFT;
  }
  if (IsKeyDown(KEY_D)) {
    new_position.x += player->player.speed * dt;
    player->player.direction = DIR_RIGHT;
  }
  if (IsKeyDown(KEY_SPACE)) {
    player->player.velocity.y = -JUMP_POWER;
    player->player.direction = DIR_UP;
  }

  /* Tile placement */
  /* TODO: Check if tile contains an entity, do damage to it and don't destroy the tile*/
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !damage_entity(player->selector, player->player.damage)) {
    if (selX >= 0 && selX < WORLD_WIDTH && selY >= 0 && selY < WORLD_HEIGHT) world->tiles[translate_index(selX, selY)].type = TypeAir_ID;
  }

  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    if (selX >= 0 && selX < WORLD_WIDTH && selY >= 0 && selY < WORLD_HEIGHT) world->tiles[translate_index(selX, selY)].type = player->selected_tile;
  }

  /* X-axis*/
  if ((check_collision_mask(world, new_position.x, player->player.position.y, playerWidth, playerHeight) & 3) == 0) {
    player->player.position.x = new_position.x;
  }

  /* Y-axis*/
  if ((check_collision_mask(world, player->player.position.x, new_position.y, playerWidth, playerHeight) & 12) == 0) {
    player->player.position.y = new_position.y;
  }

  /* World loading/saving*/
  if (IsKeyPressed(KEY_F1)) {
    saveWorld("world.dat", world);
  }
  if (IsKeyPressed(KEY_F2)) {
    loadWorld("world.dat", world);
  }
}

void update_player(Player *player, World *world, float dt) {
  float playerWidth  = PLAYER_COLLISION_W;
  float playerHeight = PLAYER_COLLISION_H;
  
  if (!player->player.onGround) player->player.accel.y = GRAVITY;
  else player->player.accel.y = 0;

  player->player.velocity.x += player->player.accel.x * dt;
  player->player.velocity.y += player->player.accel.y * dt;
  
  /* Enhanced functions*/
  float newX = player->player.position.x + player->player.velocity.x * dt;
  float newY = player->player.position.y + player->player.velocity.y * dt;
  
  /* Check if player is touching a wall, x-check*/
  
  if ((check_collision_mask(world, newX, player->player.position.y, playerWidth, playerHeight) & 3) == 0) {
    player->player.position.x = newX;
  } else {
    player->player.velocity.x = 0.0f;
  }
  
  /* Check if player is on the ground, y-check */
  if ((check_collision_mask(world, player->player.position.x, newY, playerWidth, playerHeight) & 12) == 0) {
    player->player.position.y = newY;
    player->player.onGround = false;
  } else {
    player->player.velocity.y = 0.0f;
    if (player->player.velocity.y > 0 || check_collision_mask(world, player->player.position.x, player->player.position.y + 1, playerWidth, playerHeight)) {
      player->player.onGround = true;
    }
  }
}