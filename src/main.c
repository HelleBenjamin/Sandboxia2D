#include "../include/main.h"
#include "../include/render.h"
#include "../include/mod.h"

#include <lua5.4/lauxlib.h>
#include <lua5.4/lua.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>

Mod mods[32]; /* Max of 32 mods */
int num_mods;

/* Path to the game directory, this is used to load mods, assets, etc..*/
char gamepath[512];

/* These are global variables, points to the current player, world, and camera */
Player    *current_player = NULL;
World     *current_world  = NULL;
Camera2D  *current_camera = NULL;

/* Log file*/
FILE* log_file = NULL;

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
  /* write to stdout*/
  printf("[%s] ", LOG_LEVEL_STR[type]);
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");

  /* if log file exists, write to it*/
  if (log_file) {
    fprintf(log_file, "[%s] ", LOG_LEVEL_STR[type]);
    va_start(args, fmt);
    vfprintf(log_file, fmt, args);
    va_end(args);
    fprintf(log_file, "\n");
  }
}

void raylib_log_callback(int msgType, const char* text, va_list args) {
  /* wrapper for raylib logging*/
  switch (msgType) {
    case LOG_INFO:
      gamelog(GAMELOG_INFO, text, args);
      break;
    case LOG_WARNING:
      gamelog(GAMELOG_WARN, text, args);
      break;
    case LOG_ERROR:
      gamelog(GAMELOG_ERR, text, args);
      break;
    case LOG_DEBUG:
      gamelog(GAMELOG_DEBUG, text, args);
      break;
    default: /* nothing */
      break;
  }
}
int init_gamelog() {
  char logpath[512];
  snprintf(logpath, sizeof(logpath), "%slog.txt", gamepath);
  log_file = fopen(logpath, "w");
  if (!log_file) {
    gamelog(GAMELOG_ERR, "Failed to open log file");
    return 1;
  }
  return 0;
}

void close_gamelog() {
  if (log_file) {
    fclose(log_file);
  }
}

int translate_index(int x, int y) {
  return x + y * WORLD_WIDTH;
}


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

  init_gamelog();

  gamelog(GAMELOG_INFO, "Sandboxia2D %s", VERSION_STR);

  SetTraceLogCallback(raylib_log_callback);

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

  /* Set game path */
  char self_path[512];
  size_t len = readlink("/proc/self/exe", self_path, sizeof(self_path)); /* points to the symlink, which points to the executable*/
  if (len != -1) {
    self_path[len] = '\0';
    strncpy(gamepath, self_path, sizeof(gamepath));
    /* trim */
    char *p = strrchr(gamepath, '/');
    if (p) {
      *(p+1) = '\0';
    }
  }

  gamelog(GAMELOG_INFO, "The game path is %s", gamepath);

  /* Make a constant?*/
  player.player.speed = 200.0f;
  player.player.damage = 1;
  player.selected_tile = 4;

  /* Initialize camera */
  camera.target = player.player.position;
  camera.offset = (Vector2){(float)SCREEN_WIDTH/2.0f, (float)SCREEN_HEIGHT/2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  /* Initialize renderer(eg. load textures)*/
  render_init();

  if (load_world) {
    gamelog(GAMELOG_INFO, "Loading world: %s", world_filename);
    loadWorld(world_filename, &world);
  } else { /* if no world, generate one */
    strcpy(world.name, "world");
    generateWorld(&world, -1);
  }

  float dt = 0.0f;

  /* Load mods after everything is initialized*/
  char modpath[512];
  snprintf(modpath, sizeof(modpath), "%smods", gamepath);
  gamelog(GAMELOG_INFO, "Looking for mods in: %s", modpath);
  DIR* dir = opendir(modpath);
  if (dir == NULL) {
    gamelog(GAMELOG_INFO, "No mod folder found, skipping mods");
  }
  else { 
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
      if (ends_with(entry->d_name, ".lua")) {
        char path[512];
        snprintf(path, sizeof(path), "%smods/%s", gamepath, entry->d_name);
        gamelog(GAMELOG_INFO, "Found mod: %s", path);
        mods[num_mods++] = load_mod(path);
      }
    }
  }

  /* Call init*/
  mod_call_at_start();

  while(!WindowShouldClose()) {
    dt = GetFrameTime();
    /* Call on_tick */
    mod_call_on_tick();

    /* Credit to the raylib examples*/

    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*SCREEN_WIDTH, (1 - bbox.y)*0.5f*SCREEN_HEIGHT }, camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*SCREEN_WIDTH, (1 + bbox.y)*0.5f*SCREEN_HEIGHT }, camera);
    camera.offset = (Vector2){ (1 - bbox.x)*0.5f*SCREEN_WIDTH, (1 - bbox.y)*0.5f*SCREEN_HEIGHT };

    if (player.player.position.x < bboxWorldMin.x) camera.target.x = player.player.position.x;
    if (player.player.position.y < bboxWorldMin.y) camera.target.y = player.player.position.y;
    if (player.player.position.x > bboxWorldMax.x) camera.target.x = bboxWorldMin.x + (player.player.position.x - bboxWorldMax.x);
    if (player.player.position.y > bboxWorldMax.y) camera.target.y = bboxWorldMin.y + (player.player.position.y - bboxWorldMax.y);
    
    /* Update player and world*/
    handle_input(&player, &world, &camera, dt);
    update_player(&player, &world, dt);

    render_game(&camera, &player, &world);
  }

  free(world.tiles);

  render_exit();

  for (int i = 0; i < num_mods; i++) {
    lua_close(mods[i].L);
  }

  close_gamelog();

  return 0;
}