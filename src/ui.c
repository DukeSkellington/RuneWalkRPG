#include "ui.h"
#include "game.h"
#include "world.h"
#include "combat.h"
static const char* enemy_names[3] = { "LANTERN MOTH", "SHALE WISP", "HOLLOW SENTINEL" };

static void frame(const char* title)
{
    u8 y,x;
    video_begin(); video_palette(0); video_clear();
    for(y=2;y<=27;y+=25) {
        video_address(0x2000u+((u16)y<<5)+2);
        PPUDATA=140;
        for(x=0;x<26;++x) PPUDATA=207;
        PPUDATA=140;
    }
    for(y=3;y<27;++y) {
        video_address(0x2000u+((u16)y<<5)+2); PPUDATA=143;
        video_address(0x2000u+((u16)y<<5)+29); PPUDATA=143;
    }
    video_text(4,4,title);
    video_text(4,6,"------------------------");
}
static void metatile(u8 x,u8 y,u8 id)
{
    video_address(0x2000u+((u16)y<<5)+x);
    PPUDATA=id*4; PPUDATA=id*4+1;
    video_address(0x2000u+((u16)(y+1)<<5)+x);
    PPUDATA=id*4+2; PPUDATA=id*4+3;
}
void ui_title(void)
{
    u8 x,y;
    video_begin(); video_palette(0); video_clear();
    /* Illustrated skyline, ancient walls and lake. Logo has its own palette. */
    for(x=0;x<32;x+=2) {
        metatile(x,14,(x<8 || x>24)?MOUNTAIN:BROKEN_WALL);
        metatile(x,16,OLD_GRASS);
        metatile(x,24,WATER);
        metatile(x,26,WATER);
    }
    metatile(4,12,PILLAR); metatile(26,12,PILLAR);
    metatile(14,12,LIT_RUNE);
    video_text(8,4,"THE WAKING STONES");
    for(y=0;y<3;++y) {
        video_address(0x2000u+((u16)(7+y)<<5)+5);
        for(x=0;x<21;++x) PPUDATA=144+y*21+x;
    }
    video_text(9,20,"> NEW JOURNEY");
    video_text(8,22,"PRESS START OR A");
    video_text(5,28,"AN ORIGINAL NES TALE");
    /* Attribute row 1: logo in copper. Landscape/water preserve their palettes. */
    video_address(0x23c8); for(x=0;x<8;++x) PPUDATA=0xff;
    video_address(0x23d0); for(x=0;x<8;++x) PPUDATA=0xff;
    video_address(0x23d8); for(x=0;x<8;++x) PPUDATA=0x55;
    video_address(0x23e0); for(x=0;x<8;++x) PPUDATA=0x50;
    video_address(0x23f0); for(x=0;x<8;++x) PPUDATA=0xaa;
    audio_song=0;
    video_end();
}
void ui_dialogue(const char* title,const char* a,const char* b,const char* c)
{
    frame(title);
    video_text(4,10,a); video_text(4,13,b); video_text(4,16,c);
    video_text(4,23,"A OR B - RETURN");
    video_end();
}
void ui_menu(void)
{
    frame("TRAVELER JOURNAL");
    video_text(4,8,"LEVEL"); video_number(11,8,level);
    video_text(16,8,"EXP"); video_number(21,8,experience);
    video_text(4,10,"HEALTH"); video_number(12,10,hp);
    video_text(14,10,"/"); video_number(15,10,max_hp);
    video_text(4,12,"TONICS"); video_number(12,12,tonics);
    video_text(4,15,"THE WAKING STONES");
    if(!quest) {
        video_text(4,18,"FIND THE KEEPER IN"); video_text(4,20,"BRACKEN. A AT THE GATE.");
    } else if(quest==1) {
        video_text(4,18,"SEEK GLASSMERE TO THE EAST.");
        video_text(4,20,"GUARDIANS SILENCED:"); video_number(25,20,guardians);
    } else {
        video_text(4,18,"THE FIRST STONE IS AWAKE.");
        video_text(4,20,"RETURN TO THE KEEPER.");
    }
    video_text(4,24,"B/START BACK  A USE TONIC");
    video_end();
}
void ui_battle(void)
{
    u8 x,y;
    frame("A MEMORY TAKES FORM");
    video_text(6,8,enemy_names[enemy]);
    for(y=0;y<4;++y) {
        video_address(0x2000u+((u16)(10+y)<<5)+14);
        for(x=0;x<4;++x) PPUDATA=208+enemy*16+y*4+x;
    }
    /* Enemy art: amber, distinct from the cold frame. */
    video_address(0x23d3); PPUDATA=0xf5;
    video_address(0x23db); PPUDATA=0x5f;
    video_address(0x23d4); PPUDATA=0xf5;
    video_address(0x23dc); PPUDATA=0x5f;
    video_text(6,15,"FOE"); video_number(10,15,enemy_hp);
    video_text(16,15,"HP"); video_number(19,15,hp);
    video_text(21,15,"/"); video_number(22,15,max_hp);
    if(!combat_phase) {
        video_text(7,18,"ATTACK"); video_text(7,20,"DEFEND");
        video_text(7,22,"TONIC"); video_number(14,22,tonics);
        video_text(5,18+combat_selection*2,">");
        video_text(5,25,"UP/DOWN  A CHOOSE");
    } else {
        video_text(4,19,combat_message);
        video_text(4,23,"A - CONTINUE");
        if(combat_phase==2) video_text(4,21,"EXPERIENCE AND REST GAINED.");
        if(combat_phase==3) video_text(4,21,"BRACKEN WILL SHELTER YOU.");
    }
    video_end();
}
void ui_ending(void)
{
    frame("THE FIRST STONE");
    metatile(14,8,LIT_RUNE);
    video_text(4,12,"BENEATH THE LAKE, A VOICE.");
    video_text(4,15,"NOT A WARNING. A NAME.");
    video_text(4,18,"THE KINGDOM REMEMBERS.");
    video_text(4,21,"CHAPTER ONE COMPLETE");
    video_text(4,24,"A - RETURN TO BRACKEN");
    audio_song=0;
    video_end();
}
