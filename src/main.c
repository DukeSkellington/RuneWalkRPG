#include "nes.h"
#include "game.h"
void main(void)
{
    u8 pad,old=0,pressed;
    game_init();
    for(;;) {
        video_wait();
        pad=input_read(); pressed=pad&~old; old=pad;
        game_update(pad,pressed);
        ready=1;
    }
}
