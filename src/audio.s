; NMI-only APU driver. Independent zero-page pointer, no cc65 runtime calls.
.export _audio_song, _audio_current, _sfx, _audio_beat, _audio_tick
.segment "ZEROPAGE"
audio_ptr: .res 2
.segment "BSS"
_audio_song: .res 1
_audio_current: .res 1
_audio_beat: .res 1
_sfx: .res 1
remaining: .res 1
fx_time: .res 1
fx_kind: .res 1
pulse_volume: .res 1
ended: .res 1
.segment "CODE"
_audio_tick:
    lda _audio_song
    cmp _audio_current
    beq @same
    sta _audio_current
    lda #$0f
    sta $4015
    lda #$08
    sta $4001
    sta $4005
    ldx _audio_current
    lda song_low,x
    sta audio_ptr
    lda song_high,x
    sta audio_ptr+1
    lda #0
    sta _audio_beat
    sta remaining
    sta ended
@same:
    lda ended
    beq @playing
    rts
@playing:
    lda remaining
    beq @note
    dec remaining
    jmp @mix
@note:
    ldx _audio_current
    lda song_speed,x
    sta remaining
    dec remaining
    lda _audio_beat
    cmp song_length,x
    bcc @read
    cpx #4
    bne @loop
    lda #0
    sta $4015
    lda #1
    sta ended
    rts
@loop:
    lda #0
    sta _audio_beat
    lda song_low,x
    sta audio_ptr
    lda song_high,x
    sta audio_ptr+1
@read:
    ldy #0
    lda (audio_ptr),y
    sta $4002
    iny
    lda (audio_ptr),y
    bmi @rest1
    sta $4003
    lda #$ba
    bne @volume1
@rest1:
    lda #$b0
@volume1:
    sta pulse_volume
    iny
    lda (audio_ptr),y
    sta $4006
    iny
    lda (audio_ptr),y
    bmi @rest2
    sta $4007
    lda #$76
    bne @volume2
@rest2:
    lda #$70
@volume2:
    sta $4004
    iny
    lda (audio_ptr),y
    sta $400a
    iny
    lda (audio_ptr),y
    sta $400b
    lda #$8f
    sta $4008
    iny
    lda (audio_ptr),y
    sta $400e
    lda #$08
    sta $400f
    inc _audio_beat
    clc
    lda audio_ptr
    adc #7
    sta audio_ptr
    bcc @mix
    inc audio_ptr+1
@mix:
    lda pulse_volume
    ldx remaining
    cpx #2
    bcs @envelope
    and #$f0
    ora #5
    ldx pulse_volume
    cpx #$b0
    bne @envelope
    lda #$b0
@envelope:
    sta $4000
    lda _sfx
    beq @effect
    sta fx_kind
    lda #10
    sta fx_time
    lda #0
    sta _sfx
@effect:
    lda fx_time
    beq @drums
    dec fx_time
    lda fx_time
    and #7
    eor fx_kind
    sta $400e
    lda #$3a
    sta $400c
    rts
@drums:
    lda remaining
    cmp #5
    bcc @quiet
    lda _audio_current
    beq @quiet
    cmp #2
    beq @quiet
    lda #$34
    bne @noise
@quiet:
    lda #$30
@noise:
    sta $400c
    rts
.include "build/music.inc"
