.export __STARTUP__ : absolute = 1
.export _exit, _oam
.import _main, zerobss, copydata
.import _ready, _column_pending, _column_hi, _column_lo, _column_data
.import _scroll_x, _control, _render_mask, _ticks, _water_color
.import _cursor_pending, _cursor_old, _cursor_new
.import _palette_pending, _palette_data, _animate_water
.import _attribute_data, _attribute_hi, _attribute_lo, _audio_tick
.importzp sp

.segment "HEADER"
.byte $4e,$45,$53,$1a, 2,1,1,0 ; NROM-256, CHR ROM, vertical mirroring
.byte 0,0,0,0,0,0,0,0          ; NTSC, no trainer or battery

.segment "OAM"
_oam: .res 256

.segment "STARTUP"
reset:
    sei
    cld
    ldx #$ff
    txs
    inx
    stx $2000
    stx $2001
    stx $4010
    stx $4015
    lda #$40
    sta $4017                 ; disable APU frame IRQ (and DMC DMA)
    bit $2002
@warm1:
    bit $2002
    bpl @warm1
    lda #0
    tax
@clear:
    sta $0000,x
    sta $0100,x
    sta $0200,x
    sta $0300,x
    sta $0400,x
    sta $0500,x
    sta $0600,x
    sta $0700,x
    inx
    bne @clear
@warm2:
    bit $2002
    bpl @warm2
    lda #<$0800
    sta sp
    lda #>$0800
    sta sp+1
    jsr zerobss
    jsr copydata
    jsr _main
_exit:
    jmp reset

.segment "CODE"
nmi:
    pha
    txa
    pha
    tya
    pha
    inc _ticks
    lda _ready
    bne @packet
    jmp @done
@packet:
    lda #0
    sta $2003
    lda #>_oam
    sta $4014
    lda _palette_pending
    beq @check_column
    lda #$88
    sta $2000
    bit $2002
    lda #$3f
    sta $2006
    lda #0
    sta $2006
    ldx #0
@palette:
    lda _palette_data,x
    sta $2007
    inx
    cpx #32
    bne @palette
    lda #0
    sta _palette_pending
@check_column:
    lda _column_pending
    bne @transfer
    jmp @scroll
@transfer:
    bit $2002
    lda #$84                 ; VRAM increment 32, NMI enabled
    sta $2000
    lda _column_hi
    sta $2006
    lda _column_lo
    sta $2006
    ldx #0
@column:
    lda _column_data,x
    sta $2007
    inx
    cpx #30
    bne @column
    lda #$88
    sta $2000
    .repeat 8, I
        lda _attribute_hi
        sta $2006
        lda _attribute_lo
        clc
        adc #(I*8)
        sta $2006
        lda _attribute_data+I
        sta $2007
    .endrepeat
    lda #0
    sta _column_pending
@scroll:
    lda _cursor_pending
    beq @water
    lda #$88
    sta $2000
    bit $2002
    lda #$22
    sta $2006
    lda _cursor_old
    sta $2006
    lda #96
    sta $2007
    lda #$22
    sta $2006
    lda _cursor_new
    sta $2006
    lda #141
    sta $2007
    lda #0
    sta _cursor_pending
@water:
    lda _animate_water
    beq @restore
    bit $2002
    lda #$3f
    sta $2006
    lda #$0a
    sta $2006
    lda _water_color
    sta $2007
@restore:
    bit $2002
    lda _control
    sta $2000
    lda _scroll_x
    sta $2005
    lda #0
    sta $2005
    sta _ready
    lda _render_mask
    sta $2001
@done:
    jsr _audio_tick
    pla
    tay
    pla
    tax
    pla
irq:
    rti
.segment "VECTORS"
.word nmi, reset, irq
