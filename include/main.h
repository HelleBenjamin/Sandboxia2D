#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>
#include <stdbool.h>
#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

#include "player.h"
#include "world.h"


typedef struct {
  lua_State* L;
  char name[32];
} Mod;

extern Player    *current_player;
extern World     *current_world;
extern Camera2D  *current_camera ;

extern Mod mods[32]; /* Max of 32 mods */
extern int num_mods;

extern char gamepath[512]; /* Path to the game directory */

extern int  SCREEN_WIDTH;
extern int  SCREEN_HEIGHT;
extern int  TARGET_FPS;
extern bool ENABLE_COLLISION;
extern bool ENABLE_DEBUG;
extern bool ENABLE_MODS;
extern bool ENABLE_SOUNDS;
extern int  LOG_LEVEL;

/* World signature, may vary depending on the version */
#define WORLD_SIGNATURE 0xBEEF

/* Physics constants */
#define GRAVITY     400
#define JUMP_POWER  175

/* World dimensions */
#define WORLD_WIDTH   128
#define WORLD_HEIGHT  128
#define WORLD_SIZE    (WORLD_WIDTH * WORLD_HEIGHT)

/* Entity directions */
#define DIR_UP      0
#define DIR_DOWN    1
#define DIR_LEFT    2
#define DIR_RIGHT   3

/* Collision flags */
#define COL_LEFT   (1 << 0)
#define COL_RIGHT  (1 << 1)
#define COL_TOP    (1 << 2)
#define COL_BOTTOM (1 << 3)

/* This fixes the player collision box by making it a bit smaller, change it if you want*/
#define PLAYER_COLLISION_W (TILE_SIZE * RENDER_SCALE * 0.8f)
#define PLAYER_COLLISION_H (TILE_SIZE * RENDER_SCALE * 0.98f)

/* Versioning */
#define VERSION_STR "0.2.1-beta"
#define VERSION_NUM 21
#define API_VERSION_NUM 01
#define API_VERSION_STR "Mod API v0.1"

/* Logging levels */
#define GAMELOG_NULL  0
#define GAMELOG_ERR   1 
#define GAMELOG_WARN  2
#define GAMELOG_INFO  3
#define GAMELOG_DEBUG 4
#define GAMELOG_MOD   5

/* Render constants */
#define RENDER_SCALE 5.0f
#define TILE_SIZE 8
#define FULL_TILE_SIZE TILE_SIZE * RENDER_SCALE


void gamelog(int type, const char* fmt, ...);

Mod  load_mod(const char* path);
void load_mods();

int  translate_index(int x, int y);


#endif 