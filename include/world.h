#ifndef WORLD_H
#define WORLD_H
#include "nes.h"
#define WORLD_ROWS 15u
#define WORLD_HEIGHT 240u
#define AREA_FIELD 0
#define AREA_TOWN 1
#define AREA_RUIN 2
enum { GRASS, LONG_GRASS, FLOWERS, TREE, PINE, ROCK, MOUNTAIN, WATER,
 ROOF_CENTER, ROOF_RIGHT, PATH, ROAD, BRIDGE, ROOF, WALL, DOOR, BROKEN_WALL,
 RUNE, LIT_RUNE, FLOOR, PILLAR, STAIRS, CHEST, OLD_GRASS };
extern u8 area, world_columns;
extern const u8* world_map;
extern const u8* world_attributes;
void world_select(u8 next_area);
u16 world_width(void);
u8 world_cell(u8 x, u8 y);
u8 world_solid(u16 x, u8 y);
u8 world_tile(u16 tile_x, u8 tile_y);
#endif
