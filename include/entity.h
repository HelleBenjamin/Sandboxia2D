#ifndef ENTITY_H
#define ENTITY_H

#include "world.h"
#include <lua5.4/lua.h>
#include <raylib.h>
#include <stdint.h>

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
  uint8_t damage;

  uint8_t texture_id;

  /* Behavior */
  lua_State* L;

} Entity;


extern Entity entities[0xFF];
extern int num_entities;

void update_entities(World *world, float dt);
int  create_entity(uint8_t texture_id, lua_State* L);
void free_entities();
int  damage_entity(Vector2 position, uint8_t damage);

#endif
