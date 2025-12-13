#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>
#include <stdbool.h>
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

typedef struct {
  lua_State* L;
  char name[32];
} Mod;

extern Mod mods[32]; /* Max of 32 mods */
extern int num_mods;

extern int  SCREEN_WIDTH;
extern int  SCREEN_HEIGHT;
extern int  TARGET_FPS;
extern bool ENABLE_COLLISION;
extern bool ENABLE_DEBUG;
extern bool ENABLE_MODS;
extern bool ENABLE_SOUNDS;
extern int  LOG_LEVEL;

#define VERSION_STR "0.2.0-beta"
#define VERSION_NUM 20
#define API_VERSION_NUM 01
#define API_VERSION_STR "Mod API v0.1"

#define GAMELOG_NULL  0
#define GAMELOG_ERR   1 
#define GAMELOG_WARN  2
#define GAMELOG_INFO  3
#define GAMELOG_DEBUG 4
#define GAMELOG_MOD   5

void gamelog(int type, const char* fmt, ...);

void register_lua_api(lua_State* L);
Mod load_mod(const char* path);
void load_mods();

/* Mod API functions*/
int mod_get_api_version(lua_State* L);
int mod_get_game_version(lua_State* L);

int mod_get_player_pos(lua_State* L);
int mod_set_player_pos(lua_State* L);
int mod_get_tile(lua_State* L);
int mod_set_tile(lua_State* L);
int mod_new_tile(lua_State* L);
int mod_new_texture(lua_State* L);

int mod_iskeydown(lua_State* L);
int mod_iskeypressed(lua_State* L);



#endif 