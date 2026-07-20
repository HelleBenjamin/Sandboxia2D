#ifndef MOD_H
#define MOD_H

#include <raylib.h>
#include <stdbool.h>
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

void register_lua_api(lua_State* L);

/* Mod API functions*/
int mod_get_api_version(lua_State* L);
int mod_get_game_version(lua_State* L);

int mod_get_player_pos(lua_State* L);
int mod_set_player_pos(lua_State* L);
int mod_get_tile(lua_State* L);
int mod_set_tile(lua_State* L);
int mod_new_tile(lua_State* L);
int mod_new_texture(lua_State* L);

int mod_is_key_down(lua_State* L);
int mod_is_key_pressed(lua_State* L);

int mod_check_collision(lua_State* L);
int mod_get_delta_time(lua_State* L);

/* entity related*/
int mod_spawn_entity(lua_State *L);
int mod_get_entity_pos(lua_State *L);
int mod_set_entity_pos(lua_State *L);

#endif