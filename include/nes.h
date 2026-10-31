#ifndef NES_H
#define NES_H
typedef unsigned char u8;
typedef unsigned int u16;
#define REG8(a) (*(volatile u8*)(a))
#define PPUCTRL REG8(0x2000)
#define PPUMASK REG8(0x2001)
#define PPUSTATUS REG8(0x2002)
#define PPUADDR REG8(0x2006)
#define PPUDATA REG8(0x2007)
#define JOY REG8(0x4016)
#define PAD_RIGHT 0x01
#define PAD_LEFT 0x02
#define PAD_DOWN 0x04
#define PAD_UP 0x08
#define PAD_START 0x10
#define PAD_SELECT 0x20
#define PAD_B 0x40
#define PAD_A 0x80
extern volatile u8 ready, column_pending, column_hi, column_lo;
extern volatile u8 column_data[30], attribute_data[8], attribute_hi, attribute_lo;
extern volatile u8 scroll_x, control, render_mask, ticks, water_color;
extern volatile u8 audio_song, audio_current, audio_beat, sfx;
extern volatile u8 cursor_pending, cursor_old, cursor_new;
extern u8 oam[256];
void video_init(void);
void video_wait(void);
void video_begin(void);
void video_end(void);
void video_world(u16 camera);
void video_column(u16 world_tile_column);
void video_address(u16 address);
void video_palette(u8 dark);
void video_clear(void);
void video_text(u8 x, u8 y, const char* text);
void video_number(u8 x, u8 y, u8 value);
void video_sprites_clear(void);
u8 input_read(void);
#endif
