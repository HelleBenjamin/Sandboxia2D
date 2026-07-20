#include "../include/entity.h"
#include "../include/mod.h"
#include "../include/main.h"
#include "../include/collision.h"
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
#include <raylib.h>
#include <string.h>

Entity entities[0xFF];
int num_entities = 0; /* Number of entities */

void update_entities(World *world, float dt) {
  for (int i=0; i < num_entities; i++) {
    Entity* entity = &entities[i];
    /* entity physics*/
    if (entity->collision) {
      /* same as the player*/
      if (!entity->onGround) entity->accel.y = GRAVITY;
      else entity->accel.y = 0;

      entity->velocity.x += entity->accel.x * dt;
      entity->velocity.y += entity->accel.y * dt;

      float newX = entity->position.x + entity->velocity.x * dt;
      float newY = entity->position.y + entity->velocity.y * dt;

      if ((check_collision_mask(world, newX, entity->position.y, TILE_SIZE*RENDER_SCALE, TILE_SIZE*RENDER_SCALE) & 3) == 0)
        entity->position.x = newX;
      else
        entity->velocity.x = 0.0f;

      if ((check_collision_mask(world, entity->position.x, newY, TILE_SIZE*RENDER_SCALE, TILE_SIZE*RENDER_SCALE) & 12) == 0) {
        entity->position.y = newY;
        entity->onGround = false;
      } else {
        entity->velocity.y = 0.0f;
        entity->onGround = true;
      }
    }

    /* lua script after physics done*/
    lua_getglobal(entity->L, "update");
    if (lua_isfunction(entity->L, -1)) {
      lua_pushnumber(entity->L, dt);
      if (lua_pcall(entity->L, 1, 0, 0) != LUA_OK) {
        gamelog(GAMELOG_ERR, "Entity %d update error: %s", entity->id, lua_tostring(entity->L, -1));
        lua_pop(entity->L, 1);
      }
    } else {
      lua_pop(entity->L, 1);
    }
  }
}

int create_entity(uint8_t texture_id, char* path) {
  if (num_entities > 0xFF) {
    gamelog(GAMELOG_ERR, "Maximum number of entities\n");
    return 1;
  }
  Entity* entity = &entities[num_entities];
  memset(entity, 0, sizeof(Entity));
  entity->id = num_entities;
  entity->texture_id = texture_id;
  /* setup lua*/
  entity->L = luaL_newstate();
  luaL_openlibs(entity->L);
  register_lua_api(entities[num_entities].L);
  lua_pushinteger(entity->L, entity->id);
  lua_setglobal(entity->L, "self_id"); /* script knows who is it*/

  if (luaL_dofile(entity->L, path) != LUA_OK) {
    gamelog(GAMELOG_ERR, "Entity script error: %s", lua_tostring(entity->L, -1));
    lua_pop(entity->L, 1);
    lua_close(entity->L);
    return -1;
  }

  
  return num_entities++; /* Return entity index*/
}

void free_entities() {
  for (int i=0; i < num_entities; i++) {
    lua_close(entities[i].L);
  }
}

int damage_entity(Vector2 position, uint8_t damage) {
  for (int i=0; i < num_entities; i++) {
    if (entities[i].position.x == position.x && entities[i].position.y == position.y) {
      entities[i].health -= damage;
      return i;
    }
  }
  return 0; /* No entity on the tile*/
}