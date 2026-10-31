#include "nes.h"
#include "world.h"
volatile u8 ready, column_pending, column_hi, column_lo;
volatile u8 column_data[30], attribute_data[8], attribute_hi, attribute_lo;
volatile u8 scroll_x, control, render_mask, ticks, water_color;
volatile u8 palette_pending, palette_data[32], animate_water, loading;
volatile u8 cursor_pending,cursor_old,cursor_new;
static u8 active_palette[32];
static const u8 palette[32] = {
 0x0f,0x0b,0x1a,0x29, 0x0f,0x00,0x10,0x30,
 0x0f,0x01,0x21,0x31, 0x0f,0x07,0x17,0x37,
 0x0f,0x07,0x27,0x30, 0x0f,0x0b,0x2a,0x30,
 0x0f,0x02,0x22,0x31, 0x0f,0x07,0x17,0x37
};
void video_address(u16 addr)
{
    PPUADDR = (u8)(addr >> 8); PPUADDR = (u8)addr;
}
void video_wait(void) { while (ready) {} }
void video_sprites_clear(void)
{
    u8 i=0;
    do { oam[i++] = 0xff; } while (i);
}
void video_init(void)
{
    PPUMASK=0; PPUCTRL=0;
    ready=0; render_mask=0; control=0x88; water_color=0x21;
    video_sprites_clear();
    (void)PPUSTATUS;
    PPUCTRL=0x88; /* sprites use pattern table 1; audio continues during loads */
}
static void fade_palette(u8 shade)
{
    u8 i,value,j;
    for(i=0;i<32;++i) {
        value=active_palette[i];
        for(j=0;j<shade;++j) value=value>=0x10?value-0x10:0x0f;
        palette_data[i]=value;
    }
    palette_pending=1; ready=1; video_wait();
    ready=1; video_wait();
}
void video_begin(void)
{
    u8 shade;
    video_wait(); loading=1; animate_water=0;
    if(render_mask) {
        for(shade=1;shade<=3;++shade) fade_palette(shade);
    }
    render_mask=0; ready=1; video_wait();
    column_pending=0; cursor_pending=0;
    video_sprites_clear();
    (void)PPUSTATUS;
    /* Increment by one while rendering is off; NMI skips PPU when ready=0. */
    PPUCTRL=0x88;
}
void video_end(void)
{
    render_mask=0x1e; loading=0;
    fade_palette(2); fade_palette(1); fade_palette(0);
    animate_water=1;
}
void video_palette(u8 dark)
{
    u8 i;
    video_address(0x3f00);
    for(i=0;i<32;++i) {
        active_palette[i]=palette[i];
        if(dark && i==5) active_palette[i]=0x0f;
        if(dark && i==6) active_palette[i]=0x00;
        if(dark && i==7) active_palette[i]=0x10;
        if(dark && i==10) active_palette[i]=0x11;
        if(dark && i==11) active_palette[i]=0x21;
        PPUDATA=active_palette[i];
    }
}
void video_clear(void)
{
    u16 i;
    video_address(0x2000);
    for(i=0;i<960;++i) PPUDATA=96;
    for(i=0;i<64;++i) PPUDATA=0x55;
    scroll_x=0; control=0x88;
}
static u8 character(char c)
{
    if(c==' ') return 96;
    if(c>='A' && c<='Z') return 97+c-'A';
    if(c>='0' && c<='9') return 123+c-'0';
    switch(c) {
        case '.': return 133; case ',': return 134;
        case '!': return 135; case '?': return 136;
        case ':': return 137; case '-': return 138;
        case '/': return 139; case '+': return 140;
        case '>': return 141; case '<': return 142;
    }
    return 96;
}
void video_text(u8 x,u8 y,const char* text)
{
    u8 col=0,word;
    const char* next;
    video_address(0x2000u+((u16)y<<5)+x);
    while(*text && y<30) {
        if(*text==' ') {
            next=text+1; word=0;
            while(*next && *next!=' ') { ++word; ++next; }
            if(col+word+1>29-x) {
                ++y; col=0; ++text;
                video_address(0x2000u+((u16)y<<5)+x);
                continue;
            }
        }
        if(col>=29-x) {
            ++y; col=0;
            video_address(0x2000u+((u16)y<<5)+x);
        }
        PPUDATA=character(*text); ++text; ++col;
    }
}
void video_number(u8 x,u8 y,u8 value)
{
    u8 tens=0;
    while(value>=10) { value-=10; ++tens; }
    video_address(0x2000u+((u16)y<<5)+x);
    PPUDATA=tens?123+tens:96; PPUDATA=123+value;
}
void video_world(u16 camera)
{
    u8 table,x,y,i;
    u16 physical,base;
    const u8* attrs;
    video_palette(area==AREA_RUIN);
    /* Fill ring by absolute world columns; map transitions may start mid-map. */
    base=(camera>>3)&~31u;
    for(table=0;table<2;++table) {
        physical=(base+((u16)table<<5))&63;
        video_address(0x2000u+((physical&32)?0x400:0));
        for(y=0;y<30;++y)
            for(x=0;x<32;++x) PPUDATA=world_tile(base+((u16)table<<5)+x,y);
        attrs=world_attributes+((base+((u16)table<<5))>>2)*8;
        for(y=0;y<8;++y) {
            for(i=0;i<8;++i) {
                if((base>>1)+table*16+i*2<world_columns) PPUDATA=attrs[i*8+y];
                else PPUDATA=0;
            }
        }
    }
    scroll_x=(u8)camera; control=0x88|((camera>>8)&1);
}
void video_column(u16 col)
{
    u8 y,physical,tile,cell_x,half;
    const u8* attrs;
    physical=(u8)col&63;
    column_hi=(physical&32)?0x24:0x20;
    column_lo=physical&31;
    attribute_hi=column_hi+3;
    attribute_lo=0xc0+((physical&31)>>2);
    cell_x=(u8)(col>>1); half=(u8)col&1;
    for(y=0;y<15;++y) {
        tile=world_cell(cell_x,y)*4+half;
        column_data[y*2]=tile; column_data[y*2+1]=tile+2;
    }
    attrs=world_attributes+(col>>2)*8;
    for(y=0;y<8;++y) attribute_data[y]=cell_x<world_columns?attrs[y]:0;
    column_pending=1;
}
