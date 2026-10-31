.export __STARTUP__ : absolute = 1
.export _exit, _oam
.import _main, zerobss, copydata
.import _ready, _column_pending, _column_hi, _column_lo, _column_data
.import _scroll_x, _control
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
    lda _ready
    beq @done
    lda #0
    sta $2003
    lda #>_oam
    sta $4014
    lda _column_pending
    beq @scroll
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
    lda #0
    sta _column_pending
@scroll:
    bit $2002
    lda _control
    sta $2000
    lda _scroll_x
    sta $2005
    lda #0
    sta $2005
    sta _ready
@done:
    pla
    tay
    pla
    tax
    pla
irq:
    rti
.segment "VECTORS"
.word nmi, reset, irq
