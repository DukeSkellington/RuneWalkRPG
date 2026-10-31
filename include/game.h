#ifndef GAME_H
#define GAME_H
#include "nes.h"
enum { MODE_TITLE, MODE_EXPLORE, MODE_DIALOGUE, MODE_MENU, MODE_BATTLE, MODE_ENDING };
extern u8 mode, hp, max_hp, level, experience, tonics, quest, guardians;
void game_init(void);
void game_update(u8 pad, u8 pressed);
void game_scene(u8 next_area,u16 x,u8 y);
void game_draw(void);
void game_dialogue(const char* name,const char* a,const char* b,const char* c);
void game_battle_done(u8 won);
#endif
