#include "../include/main.h"
#include "../include/render.h"
#include "../include/game.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lua.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <dirent.h>

Mod mods[32]; /* Max of 32 mods */
int num_mods;

/* These are global variables, points to the current player, world, and camera */
Player    *current_player = NULL;
World     *current_world  = NULL;
Camera2D  *current_camera = NULL;

int   SCREEN_WIDTH  = 800;
int   SCREEN_HEIGHT = 600;
int   TARGET_FPS    = 60;
bool  ENABLE_COLLISION = true;
bool  ENABLE_DEBUG  = false;
bool  ENABLE_MODS   = false;
bool  ENABLE_SOUNDS = true;
int   LOG_LEVEL     = 4;

const char* LOG_LEVEL_STR[] = {"NULL", "ERR", "WARN", "INFO", "DEBUG", "MOD"};

void gamelog(int type, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

/* MOD API */

/* This will be called once */
void mod_call_at_start() {
  for (int i = 0; i < num_mods; i++) {
    lua_getglobal(mods[i].L, "on_start");
    if (lua_isfunction(mods[i].L, -1)) {
      if(lua_pcall(mods[i].L, 0, 0, 0) != LUA_OK) {
        gamelog(GAMELOG_ERR, "Error running on_start: %s\n", lua_tostring(mods[i].L, -1));
        lua_pop(mods[i].L, 1);
      }
    } else {
      lua_pop(mods[i].L, 1);
    }    
  }
}

/* This will be called every frame */
void mod_call_on_tick() {
  for (int i = 0; i < num_mods; i++) {
    lua_getglobal(mods[i].L, "on_tick");
    if (lua_isfunction(mods[i].L, -1)) {
      if(lua_pcall(mods[i].L, 0, 0, 0) != LUA_OK) {
        gamelog(GAMELOG_ERR, "Error running on_tick: %s\n", lua_tostring(mods[i].L, -1));
        lua_pop(mods[i].L, 1);
      }
    } else {
      lua_pop(mods[i].L, 1);
    }
  }
}

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
  Vector2 pos = current_player->position;
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
  current_player->position = (Vector2){x, y};
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

void register_lua_api(lua_State* L) {
  /* Register functions */
  lua_pushcfunction(L, mod_get_api_version);
  lua_setglobal(L, "get_api_version");
  lua_pushcfunction(L, mod_get_game_version);
  lua_setglobal(L, "get_game_version");

  lua_pushcfunction(L, mod_get_player_pos);
  lua_setglobal(L, "get_player_pos");
  lua_pushcfunction(L, mod_set_player_pos);
  lua_setglobal(L, "set_player_pos");
  lua_pushcfunction(L, mod_get_tile);
  lua_setglobal(L, "get_tile");
  lua_pushcfunction(L, mod_set_tile);
  lua_setglobal(L, "set_tile");
  lua_pushcfunction(L, mod_new_tile);
  lua_setglobal(L, "new_tile");
  lua_pushcfunction(L, mod_new_texture);
  lua_setglobal(L, "new_texture");
}

Mod load_mod(const char *path) {
  Mod mod = {0};
  mod.L = luaL_newstate();
  luaL_openlibs(mod.L);

  /* Register all API functions */
  register_lua_api(mod.L);

  if (luaL_dofile(mod.L, path) != LUA_OK) { /* Check if errors*/
    gamelog(GAMELOG_ERR, "Error running %s: %s\n", path, lua_tostring(mod.L, -1));
    lua_pop(mod.L, 1);
  }

  return mod;
}


int static inline ends_with(const char* str, const char* suffix) {
  size_t lenstr = strlen(str);
  size_t lensuffix = strlen(suffix);
  if (lensuffix > lenstr) return 0;
  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));

  printf("Sandboxia2D %s\n", VERSION_STR);

  /* Main components */
  Player    player = {0};
  World     world  = {0};
  Camera2D  camera = {0};

  current_player = &player;
  current_world  = &world;
  current_camera = &camera;

  bool load_world = false;
  char *world_filename = NULL;

  for (int i = 0; i < argc; i++) {
    if (strncmp(argv[i], "--width=", 8) == 0) {
      SCREEN_WIDTH = atoi(argv[i] + 8);
    } else if (strncmp(argv[i], "--height=", 9) == 0) {
      SCREEN_HEIGHT = atoi(argv[i] + 9);
    } else if (strncmp(argv[i], "--fps=", 6) == 0) {
      TARGET_FPS = atoi(argv[i] + 6);
    } else if (strncmp(argv[i], "--collision=", 12) == 0) {
      ENABLE_COLLISION = atoi(argv[i] + 12);
    } else if (strncmp(argv[i], "--world=", 8) == 0) {
      load_world = true;
      world_filename = argv[i] + 8;
    }
  }

  /* Make a constant?*/
  player.speed = 200.0f;
  player.selected_tile = 4;

  /* Initialize camera */
  camera.target = player.position;
  camera.offset = (Vector2){(float)SCREEN_WIDTH/2.0f, (float)SCREEN_HEIGHT/2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  /* Initialize renderer(eg. load textures)*/
  render_init();

  if (load_world) {
    printf("Loading world: %s\n", world_filename);
    loadWorld(world_filename, &world);
  } else {
    strcpy(world.name, "world");
    generateWorld(&world, -1);
  }

  float dt = 0.0f;

  /* Load mods after everything is initialized*/
  DIR* dir = opendir("mods");
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    if (ends_with(entry->d_name, ".lua")) {
      char path[256];
      sprintf(path, "mods/%s", entry->d_name);
      mods[num_mods++] = load_mod(path);
      printf("Loaded mod: %s\n", entry->d_name);
    }
  }

  /* Call init*/
  mod_call_at_start();

  while(!WindowShouldClose()) {
    dt = GetFrameTime();
    handle_input(&player, &world, &camera, dt);

    /* Call on_tick */
    mod_call_on_tick();

    /* Credit to the raylib examples*/

    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*SCREEN_WIDTH, (1 - bbox.y)*0.5f*SCREEN_HEIGHT }, camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*SCREEN_WIDTH, (1 + bbox.y)*0.5f*SCREEN_HEIGHT }, camera);
    camera.offset = (Vector2){ (1 - bbox.x)*0.5f*SCREEN_WIDTH, (1 - bbox.y)*0.5f*SCREEN_HEIGHT };

    if (player.position.x < bboxWorldMin.x) camera.target.x = player.position.x;
    if (player.position.y < bboxWorldMin.y) camera.target.y = player.position.y;
    if (player.position.x > bboxWorldMax.x) camera.target.x = bboxWorldMin.x + (player.position.x - bboxWorldMax.x);
    if (player.position.y > bboxWorldMax.y) camera.target.y = bboxWorldMin.y + (player.position.y - bboxWorldMax.y);
    
    update_player(&player, &world, dt);

    /*camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.target = player.position;*/

    render_game(&camera, &player, &world);
  }

  free(world.tiles);

  render_exit();

  for (int i = 0; i < num_mods; i++) {
    lua_close(mods[i].L);
  }

  return 0;
}