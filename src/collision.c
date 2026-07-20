#include "../include/collision.h"
#include "../include/main.h"
#include <stdbool.h>
#include <math.h>

bool check_collision_box(World *world, float x, float y, float width, float height) {
  /* This checks all the bounds of the box*/
  float world_x = x / TILE_SIZE / RENDER_SCALE;
  float world_y = y / TILE_SIZE / RENDER_SCALE;
  float world_width = width / TILE_SIZE / RENDER_SCALE;
  float world_height = height / TILE_SIZE / RENDER_SCALE;
  
  int minX = (int)floorf(world_x);
  int minY = (int)floorf(world_y);
  int maxX = (int)floorf(world_x + world_width - 0.001f);
  int maxY = (int)floorf(world_y + world_height - 0.001f);
  
  for (int tileX = minX; tileX <= maxX; tileX++) {
    for (int tileY = minY; tileY <= maxY; tileY++) {
      if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT) return true; /* Out of bound check */
      if (tile_configs[world->tiles[translate_index(tileX, tileY)].type].is_solid) return true; /* Check if solid*/
    }
  }
  
  return false;
}

uint8_t check_collision_mask(World *world, float x, float y, float w, float h) {
  uint8_t mask = 0; /* Bitmask: bit 0 = left, bit 1 = right, bit 2 = top, bit 3 = bottom*/
  const float eps = 0.001f;

  /* Left */
  if (check_collision_box(world, x - eps, y, w, h)) mask |= COL_LEFT;

  /* Right */
  if (check_collision_box(world, x + eps, y, w, h)) mask |= COL_RIGHT;

  /* Top */
  if (check_collision_box(world, x, y - eps, w, h)) mask |= COL_TOP;

  /* Bottom */
  if (check_collision_box(world, x, y + eps, w, h)) mask |= COL_BOTTOM;

  return mask;
}