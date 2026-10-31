#include "world.h"

/* Six authored 16x15 screens. Symbols are terrain IDs, independent of
 * rendering and collision. Later maps can supply the same interface. */
static const char maps[6][15][17] = {
 {
 "TTTTTTTTTTTTTTTT", "T..............T", "T..TT..........T",
 "T..TT....WWW...T", "T........WWW...T", "T........WWW...T",
 "T..............T", "T+++++++++++++++", "T..............T",
 "T....OO........T", "T..........R...T", "T..............T",
 "T..TT..........T", "T..............T", "TTTTTTTTTTTTTTTT"
 }, {
 "TTTTTTTTTTTTTTTT", "T..............T", "T.WWWWW........T",
 "T.WWWWW........T", "T.WWWWW....OO..T", "T..............T",
 "T..............T", "++++++++++++++++", "T..............T",
 "T...TTTT.......T", "T...TTTT.......T", "T...........R..T",
 "T..............T", "T..............T", "TTTTTTTTTTTTTTTT"
 }, {
 "TTTTTTTTTTTTTTTT", "T......TT......T", "T......TT......T",
 "T..............T", "T..OO..........T", "T..........WWW.T",
 "T..........WWW.T", "++++++++++++++++", "T..........WWW.T",
 "T..........WWW.T", "T..............T", "T..R...........T",
 "T.....TT.......T", "T..............T", "TTTTTTTTTTTTTTTT"
 }, {
 "TTTTTTTTTTTTTTTT", "T..............T", "T..TT..TT..TT..T",
 "T..TT..TT..TT..T", "T..............T", "T..............T",
 "T..............T", "++++++++++++++++", "T..............T",
 "T..OO..........T", "T...........R..T", "T.....WWWW.....T",
 "T.....WWWW.....T", "T..............T", "TTTTTTTTTTTTTTTT"
 }, {
 "TTTTTTTTTTTTTTTT", "T..............T", "T.WWWW.........T",
 "T.WWWW.....OO..T", "T.WWWW.........T", "T..............T",
 "T..............T", "++++++++++++++++", "T..............T",
 "T..TTTT........T", "T..TTTT..R.....T", "T..............T",
 "T.........OO...T", "T..............T", "TTTTTTTTTTTTTTTT"
 }, {
 "TTTTTTTTTTTTTTTT", "T..............T", "T..TT......TT..T",
 "T..TT......TT..T", "T..............T", "T......R.......T",
 "T..............T", "+++++++++++++++T", "T..............T",
 "T.....OOOO.....T", "T..............T", "T...WWW........T",
 "T...WWW........T", "T..............T", "TTTTTTTTTTTTTTTT"
 }
};

u8 world_cell(u8 x, u8 y)
{
    char c;
    if (x >= WORLD_COLS || y >= WORLD_ROWS) return TREE;
    c = maps[x >> 4][y][x & 15];
    switch (c) {
        case 'W': return WATER;
        case 'T': return TREE;
        case '+': return PATH;
        case 'O': return ROCK;
        case 'R': return RUNE;
        default: return GRASS;
    }
}

u8 world_solid(u16 x, u8 y)
{
    u8 cell;
    if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT) return 1;
    cell = world_cell((u8)(x >> 4), y >> 4);
    return cell == WATER || cell == TREE || cell == ROCK;
}

u8 world_tile(u16 tile_x, u8 tile_y)
{
    /* Four consecutive tiles form each 16x16 metatile. */
    return world_cell((u8)(tile_x >> 1), tile_y >> 1) * 4
        + ((tile_y & 1) << 1) + (tile_x & 1);
}
