#include "../include/mod.h"
#include "../include/main.h"
#include "../include/render.h"
#include <lua5.4/lua.h>
#include <string.h>

int mod_get_api_version(lua_State *L) {
  /* Params: none */
  /* Returns: version number */
  lua_pushnumber(L, API_VERSION_NUM);
  return 1;
}

int mod_get_game_version(lua_State *L) {
  /* Params: none */
  /* Returns: version number */
  lua_pushnumber(L, VERSION_NUM);
  return 1;
}

int mod_get_player_pos(lua_State* L) {
  /* Params: none */
  /* Returns: x, y*/
  Vector2 pos = current_player->player.position;
  pos.x /= TILE_SIZE * RENDER_SCALE;
  pos.y /= TILE_SIZE * RENDER_SCALE;
  lua_pushnumber(L, pos.x);
  lua_pushnumber(L, pos.y);
  return 2;
}

int mod_set_player_pos(lua_State *L) {
  /* Params: x, y */
  /* Returns: none */
  float x = lua_tonumber(L, 1);
  float y = lua_tonumber(L, 2);
  x = x * (TILE_SIZE * RENDER_SCALE);
  y = y * (TILE_SIZE * RENDER_SCALE);
  current_player->player.position = (Vector2){x, y};
  return 0;
}

int mod_get_tile(lua_State *L) {
  /* Params: x, y */
  /* Returns: type */
  int x = lua_tonumber(L, 1);
  int y = lua_tonumber(L, 2);
  Tile tile = current_world->tiles[translate_index(x, y)];
  lua_pushnumber(L, tile.type);
  return 1;
}

int mod_set_tile(lua_State *L) {
  /* Params: x, y, type */
  /* Returns: none */
  int x = lua_tonumber(L, 1);
  int y = lua_tonumber(L, 2);
  int type = lua_tonumber(L, 3);
  current_world->tiles[translate_index(x, y)].type = type;
  return 0;
}

int mod_new_tile(lua_State *L) {
  /* Params: solid, visible, textureid, name*/
  /* Returns: tile ID */
  TileConfig config;
  config.tile_id = ++num_tile_configs;
  config.is_solid = lua_toboolean(L, 1);
  config.is_visible = lua_toboolean(L, 2);
  config.texture_id = lua_tonumber(L, 3);
  strncpy(config.name, lua_tostring(L, 4), 32);
  tile_configs[config.tile_id] = config;

  lua_pushnumber(L, config.tile_id);
  return 1; /* Tile ID*/
}

int mod_new_texture(lua_State *L) {
  /* Params: path */
  /* Returns: texture ID */
  textures[++num_textures] = LoadTexture(lua_tostring(L, 1));
  lua_pushnumber(L, num_textures);
  return 1;
}

int mod_is_key_down(lua_State *L) {
  /* Params: key */
  /* Returns: bool */
  int key = lua_tonumber(L, 1);
  lua_pushboolean(L, IsKeyDown(key));
  return 1;
}

int mod_is_key_pressed(lua_State *L) {
  /* Params: key */
  /* Returns: bool */
  int key = lua_tonumber(L, 1);
  lua_pushboolean(L, IsKeyPressed(key));
  return 1;
}

int mod_get_delta_time(lua_State *L) {
  /* Params: none */
  /* Returns: delta time */
  lua_pushnumber(L, GetFrameTime());
  return 1;
}

int mod_spawn_entity(lua_State *L) {
  /* Params: script, texture id */
  /* Returns: none*/
  char* script = (char*)lua_tostring(L, 1);
  int texture_id = lua_tointeger(L, 2);
  int id = create_entity(texture_id, script);
  lua_pushinteger(L, id);
  return 1;
}

int mod_get_entity_pos(lua_State *L) {
  /* Params: id */
  /* Returns: position*/
  int id = lua_tointeger(L, 1);
  lua_pushnumber(L, entities[id].position.x / (TILE_SIZE * RENDER_SCALE));
  lua_pushnumber(L, entities[id].position.y / (TILE_SIZE * RENDER_SCALE));
  return 2;
}

int mod_set_entity_pos(lua_State *L) {
  /* Params: position */
  /* Returns: none */
  int id = lua_tointeger(L, 1);
  entities[id].position.x = lua_tonumber(L, 2) * (TILE_SIZE * RENDER_SCALE);
  entities[id].position.y = lua_tonumber(L, 3) * (TILE_SIZE * RENDER_SCALE);
  return 0;
}

void register_lua_api(lua_State* L) {
  /* Register functions */
  /* game/api*/
  lua_pushcfunction(L, mod_get_api_version);
  lua_setglobal(L, "get_api_version");
  lua_pushcfunction(L, mod_get_game_version);
  lua_setglobal(L, "get_game_version");

  /* player */
  lua_pushcfunction(L, mod_get_player_pos);
  lua_setglobal(L, "get_player_pos");
  lua_pushcfunction(L, mod_set_player_pos);
  lua_setglobal(L, "set_player_pos");

  /* tile stuff*/
  lua_pushcfunction(L, mod_get_tile);
  lua_setglobal(L, "get_tile");
  lua_pushcfunction(L, mod_set_tile);
  lua_setglobal(L, "set_tile");
  lua_pushcfunction(L, mod_new_tile);
  lua_setglobal(L, "new_tile");
  lua_pushcfunction(L, mod_new_texture);
  lua_setglobal(L, "new_texture");

  /* keyboard stuff*/
  lua_pushcfunction(L, mod_is_key_down);
  lua_setglobal(L, "is_key_down");
  lua_pushcfunction(L, mod_is_key_pressed);
  lua_setglobal(L, "is_key_pressed");

  /* entities */
  lua_pushcfunction(L, mod_spawn_entity);
  lua_setglobal(L, "spawn_entity");
  lua_pushcfunction(L, mod_get_entity_pos);
  lua_setglobal(L, "get_entity_pos");
  lua_pushcfunction(L, mod_set_entity_pos);
  lua_setglobal(L, "set_entity_pos");
}