#include "combat.h"
#include "game.h"
#include "ui.h"
u8 enemy,enemy_hp,enemy_max,combat_selection,combat_phase,combat_turn;
const char* combat_message;
static const u8 health[3]={14,22,32};
void combat_begin(u8 foe)
{
    enemy=foe; enemy_hp=enemy_max=health[foe];
    combat_selection=0; combat_phase=0; combat_turn=0;
    mode=MODE_BATTLE; audio_song=3;
    ui_battle();
}
static void turn(void)
{
    u8 damage;
    if(combat_selection==2) {
        if(!tonics) {
            combat_message="YOUR SATCHEL IS EMPTY."; combat_phase=1; ui_battle(); return;
        }
        --tonics;
        hp=hp+16>max_hp?max_hp:hp+16;
        combat_message="A WARMTH. THEN THE CLAWS."; sfx=3;
    } else if(!combat_selection) {
        damage=6+level*2+(combat_turn&1);
        enemy_hp=enemy_hp>damage?enemy_hp-damage:0;
        combat_message="YOU STRIKE. IT ANSWERS."; sfx=2;
    } else {
        combat_message="YOU BRACE. THE BLOW FADES."; sfx=1;
    }
    ++combat_turn;
    if(!enemy_hp) {
        combat_phase=2; combat_message="THE MEMORY DISSOLVES.";
        experience+=5+enemy*4;
        if((level==1 && experience>=10)||(level==2 && experience>=22)) {
            ++level; max_hp+=6;
        }
        hp=hp+8>max_hp?max_hp:hp+8;
        audio_song=4;
    } else {
        damage=combat_selection==1?1:3+enemy*2+(combat_turn&1);
        hp=hp>damage?hp-damage:0;
        combat_phase=hp?1:3;
        if(!hp) combat_message="THE LIGHT LEAVES YOUR HAND.";
    }
    ui_battle();
}
void combat_update(u8 pressed)
{
    u8 old=combat_selection;
    if(combat_phase) {
        if(pressed&PAD_A) {
            if(combat_phase==2 || combat_phase==3) game_battle_done(combat_phase==2);
            else { combat_phase=0; ui_battle(); }
        }
        return;
    }
    if(pressed&PAD_UP) combat_selection=combat_selection?combat_selection-1:2;
    else if(pressed&PAD_DOWN) combat_selection=(combat_selection+1)%3;
    else if(pressed&PAD_A) { turn(); return; }
    else if(pressed&PAD_B) { combat_selection=1; turn(); return; }
    if(old!=combat_selection) {
        cursor_old=0x45+old*64;
        cursor_new=0x45+combat_selection*64;
        cursor_pending=1;
        sfx=1;
    }
}
