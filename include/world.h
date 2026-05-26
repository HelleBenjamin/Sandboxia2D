#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <stdbool.h>

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

enum TileIDs {
  TypeAir_ID = 0,
  TypeGrass_ID,
  TypeStone_ID,
  TypeDirt_ID,
  TypeSand_ID,
  TypeWood_ID,
  TypeLeaves_ID,
};

/* Max tile count: 256 */
extern TileConfig tile_configs[0xFF];
extern int num_tile_configs;

void generateWorld(World* world, int seed);
void saveWorld(const char* filePath, const World* world);
int  loadWorld(const char* filePath, World* world);

#endif