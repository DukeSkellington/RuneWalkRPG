#include "nes.h"
#include "player.h"
#include "camera.h"

void main(void)
{
    player_init();
    video_init();
    for (;;) {
        video_wait();
        player_update(input_read());
        camera_update(hero.x);
        player_draw(camera_x);
        ready = 1; /* Publish last; NMI owns the packet until it clears this. */
    }
}
