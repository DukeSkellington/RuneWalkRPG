#include "world.h"
#include "build/world.inc"
u8 area, world_columns;
const u8* world_map;
const u8* world_attributes;

void world_select(u8 next_area)
{
    area = next_area;
    world_columns = area == AREA_FIELD ? 128 : 32;
    switch (area) {
        case AREA_TOWN: world_map = map_1; world_attributes = attributes_1; break;
        case AREA_RUIN: world_map = map_2; world_attributes = attributes_2; break;
        default: world_map = map_0; world_attributes = attributes_0; break;
    }
}
u16 world_width(void) { return (u16)world_columns << 4; }
u8 world_cell(u8 x, u8 y)
{
    if (x >= world_columns || y >= 15) return area == AREA_RUIN ? BROKEN_WALL : PINE;
    return world_map[((u16)x << 4) + y];
}
u8 world_solid(u16 x, u8 y)
{
    if (x >= world_width() || y >= 240) return 1;
    if(area==AREA_TOWN && x>=178 && x<=189 && y>=96 && y<=111) return 1;
    return solid_table[world_cell((u8)(x >> 4), y >> 4)];
}
u8 world_tile(u16 x, u8 y)
{
    return world_cell((u8)(x >> 1), y >> 1) * 4 + ((y & 1) << 1) + (x & 1);
}
