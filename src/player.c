#include "player.h"
#include "world.h"
Player hero;

void player_init(void)
{
    hero.x = 48;
    hero.y = 112;
    hero.facing = 0;
    hero.step = 0;
    hero.moving = 0;
}

static u8 blocked(u16 x, u8 y)
{
    /* Inset the sprite: 12x8 feet box, tested at all four corners. */
    return world_solid(x + 2, y + 8) || world_solid(x + 13, y + 8)
        || world_solid(x + 2, y + 15) || world_solid(x + 13, y + 15);
}

void player_update(u8 pad)
{
    u16 x = hero.x;
    u8 y = hero.y;
    hero.moving = 0;
    /* One axis each frame: no diagonal speed boost, fixed horizontal priority.
     * Opposite directions cancel. One pixel/frame prevents tunneling. */
    if ((pad & (PAD_LEFT | PAD_RIGHT)) == PAD_LEFT) {
        if (x) --x;
        hero.facing = 2;
    } else if ((pad & (PAD_LEFT | PAD_RIGHT)) == PAD_RIGHT) {
        if (x < WORLD_WIDTH - 16) ++x;
        hero.facing = 3;
    } else if ((pad & (PAD_UP | PAD_DOWN)) == PAD_UP) {
        if (y) --y;
        hero.facing = 1;
    } else if ((pad & (PAD_UP | PAD_DOWN)) == PAD_DOWN) {
        if (y < WORLD_HEIGHT - 16) ++y;
        hero.facing = 0;
    }
    if (!blocked(x, y)) {
        hero.moving = x != hero.x || y != hero.y;
        hero.x = x;
        hero.y = y;
    }
    if (hero.moving) ++hero.step;
}

void player_draw(u16 camera)
{
    u8 x = (u8)(hero.x - camera);
    u8 tile = 32 + hero.facing * 8;
    u8 i;
    if (hero.moving && (hero.step & 8)) tile += 4;
    for (i = 0; i < 4; ++i) {
        oam[i * 4] = hero.y - 1 + ((i & 2) ? 8 : 0);
        oam[i * 4 + 1] = tile + i;
        oam[i * 4 + 2] = 0;
        oam[i * 4 + 3] = x + ((i & 1) ? 8 : 0);
    }
}
