# Sandboxia2D Modding Guide

This is a guide for modding Sandboxia2D with Lua. It's partially documented, but it's still a work in progress.

## Why Lua?

I chose it because it's a simple language that is easy to learn and understand. Also, it's very portable, so you can use it on any platform without any recompilation.

## API functions in Lua

### get_api_version() -> api_version

Returns the version of the modding API. Useful for checking if the mod is compatible with the game.

### get_game_version() -> game_version

Returns the version of the game. Useful for checking if the mod is compatible with the game.

### get_player_pos() -> x, y

Returns the position of the player in floats.

### set_player_pos(x, y) -> none

Sets the position of the player.

### get_tile(x, y) -> tile_type

Returns the type of the tile at the given coordinates.

### set_tile(x, y, tile_type) -> none

Sets the type of the tile at the given coordinates.

### new_tile(solid, visible, texture_id, name) -> tile_id

Creates a new tile with the given parameters and returns its ID.

### new_texture(path) -> texture_id

Creates a new texture with the given path and returns its ID. Note: you don't need to manually free the texture, the game will do it for you.

### is_key_down(key) -> bool

Returns true if the given key is down. The key parameter is ASCII.

### is_key_pressed(key) -> bool

Returns true if the given key is pressed. The key parameter is ASCII.

### get_delta() -> delta

Returns the delta time since the last frame.


## Entity behavior functions in Lua
Each entity behavior is defined in a Lua file. All entities have the same functions as defined in the API.


All entites must have the following functions:

### init() -> none

Called when the entity is created.

### update() -> none

Called every frame.

### damage() -> none

Called when the entity is damaged.

These are extras:

### move_relative(dir, tiles) -> none

Moves the entity in the given direction by the given number of tiles.

### move_absolute(x, y) -> none

Moves the entity to the given position.

### get_pos() -> x, y

Returns the position of the entity.

### get_collision_mask() -> collision_mask

Returns the collision mask of the entity.