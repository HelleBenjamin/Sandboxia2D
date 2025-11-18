#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>
#include <stdbool.h>

extern int  SCREEN_WIDTH;
extern int  SCREEN_HEIGHT;
extern bool ENABLE_VSYNC;
extern bool ENABLE_COLLISION;
extern bool ENABLE_DEBUG;
extern bool ENABLE_MODS;
extern bool ENABLE_SOUNDS;
extern int  LOG_LEVEL;

#define VERSION_STR "0.2.0-beta"
#define VERSION_NUM 20

void gamelog(int type, const char* fmt, ...);

#define GAMELOG_NULL  0
#define GAMELOG_ERR   1 
#define GAMELOG_WARN  2
#define GAMELOG_INFO  3
#define GAMELOG_DEBUG 4
#define GAMELOG_MOD   5

#endif 