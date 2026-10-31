#ifndef WORLD_H
#define WORLD_H
#include "nes.h"
#define WORLD_COLS 96u
#define WORLD_ROWS 15u
#define WORLD_WIDTH (WORLD_COLS * 16u)
#define WORLD_HEIGHT 240u
enum { GRASS, WATER, TREE, PATH, ROCK, RUNE };
u8 world_cell(u8 x, u8 y);
u8 world_solid(u16 x, u8 y);
u8 world_tile(u16 tile_x, u8 tile_y);
#endif
