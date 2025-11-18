#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdint.h>

#define WORLD_SIGNATURE 0xBEEF

#define GRAVITY     400
#define JUMP_POWER  175

#define WORLD_WIDTH 128
#define WORLD_HEIGHT 128

typedef struct {
  uint8_t type;   /* Tile type */
  uint8_t state;  /* Tile state, eg. lit, open */
} Tile;

typedef struct {
  char name[32];  /* World name */
  uint32_t seed;  /* World seed */
  uint32_t size;  /* World size */
  Tile* tiles;    /* World tiles */ 
} World;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  Vector2 accel;
  float speed;
  bool onGround;
  uint8_t direction;
  Vector2 selector;
  uint8_t selected_tile;
} Player;

enum TileID {
  TypePlayer = 0,
  TypePlayer_Left,
  TypePlayer_Right,
  TypeSelector,
  TypeAir, /* Placeable tiles start here */
  TypeGrass,
  TypeStone,
  TypeDirt,
  TypeSand,
  TypeWood,
  TypeLeaves,
};

// Max tile count: 256
extern Tile Tiles[0xFF];

void generateWorld(World* world, int seed);
void saveWorld(const char* filePath, const World* world);
int loadWorld(const char* filePath, World* world);

int translate_index(int x, int y);
bool check_collision(World *world, float x, float y);

void update_player(Player* player, World* world, float dt);

void handle_input(Player* player, World* world, float dt);

#endif