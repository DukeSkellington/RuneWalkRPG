#include "camera.h"
#include "world.h"
u16 camera_x;

void camera_update(u16 target_x)
{
    u16 next, old_column, new_column;
    next = target_x > 120 ? target_x - 120 : 0;
    if (next > WORLD_WIDTH - 256) next = WORLD_WIDTH - 256;
    old_column = camera_x >> 3;
    new_column = next >> 3;
    if (new_column > old_column) {
        /* Ahead of the visible edge; the two nametables form a 512px ring. */
        video_column(new_column + 32);
    } else if (new_column < old_column && new_column) {
        video_column(new_column - 1);
    }
    camera_x = next;
    scroll_x = (u8)camera_x;
    control = 0x80 | ((camera_x >> 8) & 1);
}
