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
#define PAD_LEFT  0x02
#define PAD_DOWN  0x04
#define PAD_UP    0x08
/* NMI consumes a complete frame packet only after ready is set last.
 * Never modify this packet until ready returns to zero. */
extern volatile u8 ready, column_pending, column_hi, column_lo;
extern volatile u8 column_data[30], scroll_x, control;
extern u8 oam[256];
void video_init(void);
void video_wait(void);
void video_column(u16 world_tile_column);
u8 input_read(void);
#endif
