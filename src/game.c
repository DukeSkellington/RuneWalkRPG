#include "game.h"
#include "player.h"
#include "camera.h"
#include "world.h"
#include "ui.h"
#include "combat.h"
u8 mode,hp,max_hp,level,experience,tonics,quest,guardians;
static u8 chest_taken;
static u8 scene_music(void) { return area==AREA_RUIN?2:1; }

void game_draw(void)
{
    u8 i;
    u16 keeper=176;
    player_draw(camera_x);
    if(area==AREA_TOWN && keeper>=camera_x && keeper-camera_x<240) {
        for(i=0;i<4;++i) {
            oam[16+i*4]=95+((i&2)?8:0);
            oam[17+i*4]=64+i;
            oam[18+i*4]=1;
            oam[19+i*4]=(u8)(keeper-camera_x)+((i&1)?8:0);
        }
    }
}
static void explore(void)
{
    mode=MODE_EXPLORE;
    video_begin(); video_world(camera_x); game_draw();
    audio_song=scene_music(); video_end();
}
void game_scene(u8 next_area,u16 x,u8 y)
{
    world_select(next_area);
    hero.x=x; hero.y=y; hero.moving=0;
    camera_x=x>120?x-120:0;
    if(camera_x>world_width()-256) camera_x=world_width()-256;
    explore();
}
void game_dialogue(const char* name,const char* a,const char* b,const char* c)
{
    mode=MODE_DIALOGUE; sfx=1; ui_dialogue(name,a,b,c);
}
void game_init(void)
{
    mode=MODE_TITLE; audio_current=255; audio_song=0;
    video_init(); ui_title();
}
static u8 near_point(u16 x,u8 y)
{
    u16 dx=hero.x>x?hero.x-x:x-hero.x;
    u8 dy=hero.y>y?hero.y-y:y-hero.y;
    return dx<=24 && dy<=24;
}
static void interact(void)
{
    if(area==AREA_FIELD) {
        if(near_point(192,112)) { game_scene(AREA_TOWN,48,112); return; }
        if(near_point(1632,112)) {
            if(!quest) game_dialogue("A SEALED PASSAGE","THE STONE HOLDS ITS BREATH.","SOMEONE IN BRACKEN MAY KNOW.","THE SETTLEMENT LIES WEST.");
            else game_scene(AREA_RUIN,48,112);
            return;
        }
        game_dialogue("THE OLD ROAD","A - SPEAK OR EXAMINE","START - JOURNAL AND TONICS","FOLLOW THE ROAD EAST.");
    } else if(area==AREA_TOWN) {
        if(near_point(16,112)) { game_scene(AREA_FIELD,208,112); return; }
        if(near_point(176,96)) {
            hp=max_hp;
            if(tonics<3) tonics=3;
            if(!quest) {
                quest=1;
                game_dialogue("KEEPER OF BRACKEN","THE STONES STIR IN THEIR SLEEP.","SEEK GLASSMERE BEYOND THE RIVER.","TAKE THESE TONICS. RETURN SAFE.");
            } else if(quest==1) {
                game_dialogue("KEEPER OF BRACKEN","THE ROAD CROSSES TWO RIVERS.","AT GLASSMERE, EXAMINE THE RUNE.","REST HERE. YOUR LIGHT RETURNS.");
            } else {
                game_dialogue("KEEPER OF BRACKEN","I HEARD IT, TOO. THE OLD NAME.","THERE ARE OTHER STONES, TRAVELER.","FOR TONIGHT, LET BRACKEN REST.");
            }
            return;
        }
        if(near_point(128,96)) game_dialogue("THE VILLAGE STONE","THE CARVING IS WARM TO TOUCH.","FLOWERS GROW THROUGH ITS CRACKS.","NO ONE HERE WILL CUT THEM.");
        else game_dialogue("BRACKEN","A FEW ROOFS AGAINST THE FOREST.","THE KEEPER WAITS BY THE PLAZA.","A AT THE WEST STAIR TO LEAVE.");
    } else {
        if(near_point(16,112)) { game_scene(AREA_FIELD,1648,112); return; }
        if(near_point(112,80)) {
            if(chest_taken) {
                game_dialogue("AN OLD OFFERING","ONLY A DRIED FLOWER REMAINS.","ITS COLOR HAS NOT FADED.","YOU LEAVE IT IN THE CHEST.");
                return;
            }
            if(tonics<5) ++tonics;
            chest_taken=1;
            game_dialogue("AN OLD OFFERING","A SEALED TONIC, STILL WARM.","THE CHEST KEEPS ONE FOR YOU.","THE REST BELONG TO THE LOST.");
            return;
        }
        if(near_point(352,96)) {
            if(guardians<3) combat_begin(guardians);
            else { quest=2; mode=MODE_ENDING; ui_ending(); }
            return;
        }
        game_dialogue("GLASSMERE","THE SILENCE HAS A HEARTBEAT.","THREE MEMORIES GUARD THE RUNE.","EXAMINE IT WITH A.");
    }
}
void game_battle_done(u8 won)
{
    if(won) { ++guardians; explore(); }
    else { hp=max_hp; if(!tonics) tonics=1; game_scene(AREA_TOWN,160,112); }
}
void game_update(u8 pad,u8 pressed)
{
    u8 i;
    water_color=(ticks&32)?0x21:0x31;
    switch(mode) {
        case MODE_TITLE:
            for(i=0;i<4;++i) {
                oam[i*4]=32+i*9;
                oam[i*4+1]=68+((ticks>>4)+i)%4;
                oam[i*4+2]=2;
                oam[i*4+3]=24+i*61;
            }
            for(i=0;i<4;++i) {
                oam[16+i*4]=7+((i&2)?8:0);
                oam[17+i*4]=72+i;
                oam[18+i*4]=2;
                oam[19+i*4]=120+((i&1)?8:0);
            }
            if(pressed&(PAD_START|PAD_A)) {
                hp=max_hp=24; level=1; experience=0; tonics=3; quest=0; guardians=0; chest_taken=0;
                player_init(); game_scene(AREA_FIELD,80,112);
                game_dialogue("THE WAKING STONES","YOU ARRIVE AS THE BELLS FALL QUIET.","ASK THE KEEPER IN BRACKEN.","A AT THE SETTLEMENT GATE.");
            }
            break;
        case MODE_EXPLORE:
            if(pressed&PAD_START) { mode=MODE_MENU; ui_menu(); }
            else if(pressed&PAD_A) interact();
            else { player_update(pad); camera_update(hero.x); game_draw(); }
            break;
        case MODE_DIALOGUE:
            if(pressed&(PAD_A|PAD_B)) explore();
            break;
        case MODE_MENU:
            if(pressed&(PAD_B|PAD_START)) explore();
            else if((pressed&PAD_A) && tonics && hp<max_hp) {
                --tonics; hp=hp+16>max_hp?max_hp:hp+16; sfx=3; ui_menu();
            }
            break;
        case MODE_BATTLE: combat_update(pressed); break;
        case MODE_ENDING:
            if(pressed&PAD_A) game_scene(AREA_TOWN,160,112);
            break;
    }
}
