#ifndef COMBAT_H
#define COMBAT_H
#include "nes.h"
extern u8 enemy, enemy_hp, enemy_max, combat_selection, combat_phase, combat_turn;
extern const char* combat_message;
void combat_begin(u8 foe);
void combat_update(u8 pressed);
#endif
