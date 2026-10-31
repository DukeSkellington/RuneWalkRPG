# Mesen and original-hardware acceptance checklist

Use a maintained desktop release of [Mesen](https://www.mesen.ca/) from the
[SourMesen/Mesen2 project](https://github.com/SourMesen/Mesen2). Load
`dist/rune-walk.nes` with File → Open. Set NTSC timing, normal speed, an NES
controller in port 1, and no cheats. Keep `build/rune-walk.dbg` beside a locally
built ROM for cc65 source symbols; `.lbl` and `.map` are also generated.

## Presentation and sound

- Cold boot shows the Rune Walk logo, moon and stars, skyline and lake. Only New
  Journey is offered. Start/A fades into the chapter introduction.
- Listen through the title and field themes' complete loops. Check melody,
  pulse harmony, triangle bass, noise rhythm, transitions and relative levels.
- Water shimmers and the hero animates in every direction, settling into a
  direction-specific standing pose on release. Look at 1× integer scale as well
  as a larger window; the silhouette should remain readable.
- Scene changes fade to black without garbage tiles. Dialogue wraps inside the
  frame. Start opens an objective journal. Battle cursor changes are immediate.

## Playability

- Complete the route described in README: keeper, two bridges, ruin, three
  guardians, awakened stone, return. Try starting at Glassmere before speaking
  to the keeper; the passage must remain sealed.
- Walk into water from both banks, through forest corners, into rocks, buildings,
  pillars and the NPC. Only walkable tiles should admit the feet box.
- Reach both overworld boundaries; reverse at every 256-pixel crossing and on
  bridge edges. No stale graphics or palette quadrants should appear.
- Try opposite directions, both axes, held A and held Start. Dialogue/menu actions
  must trigger on new presses, not repeatedly while a button stays held.
- Collect the chest twice: the second inspection must not give another tonic.
- Attack, defend, use a tonic and exhaust the satchel. Check HP arithmetic, victory
  experience, leveling, three guardian flags, defeat recovery and the conclusion.
- Hold on a victory long enough to hear the fanfare end; field music resumes
  after confirming. Reset discards progress and returns to the title by design.

## Hardware diagnostics

- Confirm mapper 0, vertical mirroring, 32 KiB PRG and 8 KiB CHR ROM. Sprite
  patterns use table 1 and backgrounds table 0. There is no cartridge RAM.
- Enable invalid PPU-access diagnostics if the Mesen version provides them.
  Runtime nametable/attribute/palette writes should occur during vblank; bulk
  boot and map reload writes occur only with rendering disabled.
- Inspect NMI in the profiler, especially streaming plus music-note changes.
  There are no mapper IRQs, sprite-zero splits or DMC DMA assumptions.
- Enable randomized power-on memory if supported and repeat cold boots. Verify
  controller reads and audio on original NTSC speed; no overclocking.

Copy the ROM to an EverDrive N8 Pro SD card, launch it through the installed
EverDrive OS on an NTSC NES, and repeat the route, audio and reset checks. No
special mapper file or save file is needed. Check overscan and the NES's actual
composite palette on a television. Physical testing and these Mesen-specific
checks remain outstanding; JSNES execution does not establish either result.
