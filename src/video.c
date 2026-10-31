#include "nes.h"
#include "world.h"

volatile u8 ready, column_pending, column_hi, column_lo;
volatile u8 column_data[30], scroll_x, control;

static const u8 palette[32] = {
    0x0f,0x19,0x21,0x38, 0x0f,0x19,0x21,0x38,
    0x0f,0x19,0x21,0x38, 0x0f,0x19,0x21,0x38,
    0x0f,0x16,0x27,0x30, 0x0f,0x16,0x27,0x30,
    0x0f,0x16,0x27,0x30, 0x0f,0x16,0x27,0x30
};

static void address(u16 addr)
{
    PPUADDR = (u8)(addr >> 8);
    PPUADDR = (u8)addr;
}

void video_init(void)
{
    u8 table, x, y, i;
    PPUMASK = 0;
    PPUCTRL = 0;
    (void)PPUSTATUS;
    address(0x3f00);
    for (i = 0; i < 32; ++i) PPUDATA = palette[i];
    for (table = 0; table < 2; ++table) {
        address(0x2000u + ((u16)table << 10));
        for (y = 0; y < 30; ++y)
            for (x = 0; x < 32; ++x)
                PPUDATA = world_tile(((u16)table << 5) + x, y);
        /* One terrain palette: streaming columns need no attribute writes. */
        for (i = 0; i < 64; ++i) PPUDATA = 0;
    }
    i = 0;
    do { oam[i++] = 0xff; } while (i);
    control = 0x80;
    scroll_x = 0;
    ready = 1;
    /* Start on a fresh vblank; NMI uploads the first frame before drawing. */
    while (!(PPUSTATUS & 0x80)) {}
    PPUCTRL = 0x80;
    video_wait();
    PPUMASK = 0x1e;
}

void video_wait(void)
{
    while (ready) {}
}

void video_column(u16 world_tile_column)
{
    u8 y, physical, tile, cell_x, half;
    physical = (u8)world_tile_column & 63;
    column_hi = (physical & 32) ? 0x24 : 0x20;
    column_lo = physical & 31;
    cell_x = (u8)(world_tile_column >> 1);
    half = (u8)world_tile_column & 1;
    for (y = 0; y < 15; ++y) {
        tile = world_cell(cell_x, y) * 4 + half;
        column_data[y * 2] = tile;
        column_data[y * 2 + 1] = tile + 2;
    }
    column_pending = 1;
}
