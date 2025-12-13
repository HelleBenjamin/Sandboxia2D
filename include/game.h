#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdint.h>

#define WORLD_SIGNATURE 0xBEEF

#define GRAVITY     400
#define JUMP_POWER  175

#define WORLD_WIDTH   128
#define WORLD_HEIGHT  128
#define WORLD_SIZE    (WORLD_WIDTH * WORLD_HEIGHT)

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
  Vector2 position;
  Vector2 velocity;
  Vector2 accel;
  float speed;
  bool onGround;
  uint8_t direction;
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

void generateWorld(World* world, int seed);
void saveWorld(const char* filePath, const World* world);
int loadWorld(const char* filePath, World* world);

int translate_index(int x, int y);
bool check_collision(World *world, float x, float y);

void update_player(Player* player, World* world, float dt);

void handle_input(Player* player, World* world, Camera2D* camera, float dt);

#endif