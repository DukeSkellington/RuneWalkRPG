# Cartridge, engine contracts and limitations

## Why NROM remains

The opening chapter fits NROM-256 (mapper 0), a standard EverDrive N8 Pro target.
There is no bank-switch hardware, external RAM, battery, expansion audio or PAL
compatibility claim. The iNES 1.0 header declares two 16 KiB PRG banks, one 8 KiB
CHR bank, vertical mirroring, NTSC, no trainer and no battery. The legacy iNES RAM
byte is zero; the game never accesses cartridge RAM regardless of an emulator's
legacy default interpretation.

The build uses all eight physical KiB of the CHR-ROM file, but many sprite-pattern
slots contain zeroes. **The background table is full; the whole CHR ROM is not.**
A future scene-specific pattern-table layout is worth exploring before changing
mapper. MMC3 is not necessary to run this chapter.

## CHR layout

| Pattern table | Tile indices | Contents |
| --- | --- | --- |
| Background `$0000` | 0–95 | 24 four-tile terrain metatiles |
| Background | 96–142 | 47 original text glyphs |
| Background | 143, 207 | Continuous frame borders |
| Background | 144–206 | 21×3-tile original title logo |
| Background | 208–255 | Three 4×4-tile enemy portraits |
| Sprites `$1000` | 0–63 | Four directions × four traveler poses × four tiles |
| Sprites | 64–67 | Keeper |
| Sprites | 68–71 | Star animation |
| Sprites | 72–75 | Moon |
| Sprites | 76–255 | Reserved, currently blank |

The hero and keeper are 16×16 four-sprite actors. Even overlapping them uses four
sprites per scanline, below the hardware limit of eight. Title stars and the moon
also stay within that limit. Enemy portraits are background tiles, not oversized
sprite groups. There is no sprite multiplexing or intentional sprite flicker.

## CPU RAM and interrupt contract

- `$0002–$001D`: 28 zero-page bytes for cc65 and the audio driver's private pointer.
- `$0100–$01FF`: hardware stack; reset initializes SP to `$FF`.
- `$0200–$02FF`: aligned 256-byte shadow OAM.
- `$0300–$05FF`: linker-bounded static game data and frame buffers. Current DATA
  plus BSS uses 165 bytes, ending at `$03A4`.
- `$0600–$07FF`: 512-byte cc65 software stack, initialized at `$0800` and descending.

NMI preserves A/X/Y and never calls C or uses cc65's zero-page temporaries. Audio
uses its own pointer and exclusively owns its internal playback state. Main may
only publish the song/effect request bytes. Main owns the render packet while
`ready=0`; it publishes `ready=1` only after completing every field. NMI uploads
and then releases it. NMI's audio work runs even when no render packet is ready.

Reset clears internal RAM, waits for PPU warmup, disables frame and DMC IRQs,
initializes the C data segment, and enables rendering only through a complete
frame packet. There are no runtime heap allocations or recursive game functions.

## Scrolling and transitions

Each map is 15 metatiles high. The field is 128 metatiles wide (2048×240 pixels);
Bracken and Glassmere are each 32 wide (512×240). There is horizontal scrolling
and four-direction movement, not a vertically scrolling world.

The nametables form a 512-pixel circular buffer. A streaming packet carries a
30-tile column and eight prepacked attribute bytes, plus OAM and scroll state.
The column ahead of the camera is generated once per eight-pixel crossing;
attribute quadrants match the original map on both directions of travel.

This update path assumes no camera jumps larger than one tile column. Map changes,
return from dialogue and recovery explicitly disable rendering and refill both
nametables around the absolute camera position. Do not teleport a camera through
`camera_update` and expect the ring to repair itself.

Palette fades use a separate 32-byte NMI queue. Fade and bulk scene loads run
with no pending column update. Battle cursors use a two-cell queue. The current
runtime test's latest exploration scroll write is at vblank line 11 of 20 in
JSNES; audio runs after PPU restoration. This is emulator evidence, not a measured
hardware timing certificate. Any future larger queue needs a fresh budget check.

## Audio

The NMI driver updates NES pulse 1, pulse 2, triangle and noise registers. Music
uses scored timer records with explicit rests, pulse release taper, harmony,
bass and percussion. Four themes loop; victory stops after one pass. A noise
sound effect temporarily overrides percussion, then the score regains that voice.
No DPCM is enabled, avoiding its controller-read DMA hazard.

The title and field have 16-bar forms. Ruin and battle are arrangements of those
motifs; they are not claimed as equally finished independent compositions.
There is no tracker importer, vibrato, stereo panning or advanced instrument
macro system. Real NES output is mono. MP3 evidence is an encoded JSNES capture,
not an audio recording from a console.

## Gameplay scope

One chapter, one hero, one keeper, one finite chest, three fixed encounters, two
basic commands plus a healing consumable, two possible level gains, and a chapter
ending. No saving, equipment, shops, random encounters, interiors, autonomous NPC
AI or multi-floor dungeon exists yet. A reset discards progress. The journal and
keeper provide the objective; interactions use proximity rather than strict
facing. Defeat preserves the objective and already-cleared guardians.

Dialogue is a full-screen modal scene; the renderer wraps words within its frame.
It does not page arbitrary-length dialogue, localize text or restore a partial
in-world dialogue overlay. Scene/message changes are deliberately synchronous,
with fades; movement does not run during them. This is a clear next area for
presentation work rather than an unbounded text engine.
