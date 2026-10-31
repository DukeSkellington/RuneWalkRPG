#include "camera.h"
#include "world.h"
u16 camera_x;
void camera_update(u16 target_x)
{
    u16 next,old_column,new_column;
    next=target_x>120?target_x-120:0;
    if(next>world_width()-256) next=world_width()-256;
    old_column=camera_x>>3; new_column=next>>3;
    if(new_column>old_column) video_column(new_column+32);
    else if(new_column<old_column && new_column) video_column(new_column-1);
    camera_x=next; scroll_x=(u8)next; control=0x88|((next>>8)&1);
}
