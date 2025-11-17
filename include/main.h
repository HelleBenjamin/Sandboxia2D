#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <string>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool VSYNC;
extern bool COLLISION;
extern bool DEBUG;
extern bool MODS_ENABLED;
extern bool SOUNDS_ENABLED;
extern int LOG_LEVEL;

#define VERSION_STR "0.1.9-beta"
#define VERSION_NUM 19

std::string handleConsoleCommand(std::string command); // Returns the output of the command to the console

int initGame();
int main(int argc, char *argv[]);
void log(int type, const char *fmt, ...);

#define SLOG_NULL  0
#define SLOG_ERR   1 
#define SLOG_WARN  2
#define SLOG_INFO  3
#define SLOG_DEBUG 4
#define SLOG_MOD   5

#endif