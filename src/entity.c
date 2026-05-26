#include "../include/entity.h"
#include "../include/mod.h"
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

Entity entities[0xFF];
int num_entities = 0; /* Number of entities */

void update_entities(World *world, float dt) {
  for (int i=0; i < num_entities; i++) {
    lua_getglobal(entities[i].L, "update");
    if (lua_isfunction(entities[i].L, -1)) {
      lua_pushnumber(entities[i].L, dt);
      lua_pcall(entities[i].L, 1, 0, 0);
    }
  }
}

int create_entity(uint8_t texture_id, lua_State* L) {
  entities[num_entities].id = num_entities;
  entities[num_entities].texture_id = texture_id;

  /* Lua setup */
  entities[num_entities].L = L;
  entities[num_entities].L = luaL_newstate();
  luaL_openlibs(entities[num_entities].L);

  /* Give all the mod functions*/
  register_lua_api(entities[num_entities].L);



  num_entities++;
  return num_entities - 1; /* Return entity index*/
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