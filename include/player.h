#ifndef PLAYER_H
#define PLAYER_H
#include "nes.h"
typedef struct { u16 x; u8 y; u8 facing; u8 moving; u8 step; } Player;
extern Player hero;
void player_init(void);
void player_update(u8 pad);
void player_draw(u16 camera);
#endif
