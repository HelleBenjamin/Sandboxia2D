#ifndef GAME_H
#define GAME_H

#include <lua5.4/lua.h>
#include <raylib.h>
#include <stdint.h>

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


typedef struct {
  uint8_t tile_id;    /* Tile ID */
  bool is_solid;      /* Is tile solid, used for collision */
  bool is_visible;    /* Is tile visible, used for rendering */
  uint8_t texture_id; /* Texture of tile, used for rendering */
  char name[32];      /* Tile name, eg. grass, stone */
} TileConfig;


typedef struct {
  uint8_t type;       /* Tile type */
  uint8_t state;      /* Tile state in bitfield, bit 0: lit, bit 1: open, etc.*/
} Tile;

typedef struct {
  char name[32];    /* World name */
  uint32_t seed;    /* World seed */
  uint32_t size;    /* World size */
  uint16_t width;   /* World width */
  uint16_t height;  /* World height */
  Tile* tiles;      /* World tiles */ 
} World;

typedef struct {
  uint32_t id;

  Vector2 position;
  Vector2 velocity;
  Vector2 accel;
  float speed;
  bool onGround;
  bool collision;
  uint8_t direction;

  uint32_t flags;
  uint8_t health;

  /* Behavior */
  lua_State* L;

} Entity;

typedef struct {
  Entity  player;
  Vector2 selector;
  uint8_t selected_tile;
} Player;


enum TextureIDs {
  TypePlayer_Texture = 0,
  TypePlayer_Left_Texture,
  TypePlayer_Right_Texture,
  TypeSelector_Texture,
  TypeAir_Texture, /* Placeable tiles start here */
  TypeGrass_Texture,
  TypeStone_Texture,
  TypeDirt_Texture,
  TypeSand_Texture,
  TypeWood_Texture,
  TypeLeaves_Texture,
};

enum TileIDs {
  TypeAir_ID = 0,
  TypeGrass_ID,
  TypeStone_ID,
  TypeDirt_ID,
  TypeSand_ID,
  TypeWood_ID,
  TypeLeaves_ID,
};

// Max tile count: 256
extern TileConfig tile_configs[0xFF];
extern int num_tile_configs;

extern Entity entities[0xFF];
extern int num_entities;

void generateWorld(World* world, int seed);
void saveWorld(const char* filePath, const World* world);
int loadWorld(const char* filePath, World* world);

int translate_index(int x, int y);
bool check_collision(World *world, float x, float y);

void update_player(Player* player, World* world, float dt);

void handle_input(Player* player, World* world, Camera2D* camera, float dt);

#endif