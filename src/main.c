#include "../include/main.h"
#include "../include/render.h"
#include "../include/game.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

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

int main(int argc, char* argv[]) {
  srand(time(NULL));

  printf("Sandboxia2D %s\n", VERSION_STR);

  /* Main components */
  Player    player = {0};
  World     world  = {0};
  Camera2D  camera = {0};

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
  while(!WindowShouldClose()) {
    dt = GetFrameTime();
    handle_input(&player, &world, &camera, dt);

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

  return 0;
}