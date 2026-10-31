# Rune Walk

A small, original NES homebrew foundation for a future RPG. Walk a red-cloaked
hero through a six-screen overworld of roads, trees, water, rocks, and rune stones.
The D-pad moves in four directions; the camera follows horizontally. Trees,
water, rocks, and the world edges block movement. Runes are decorative for now.
A/B/Start/Select have no action. There is no audio or save system yet.

## Download the ROM

The prebuilt ROM is checked in at [`dist/rune-walk.nes`](dist/rune-walk.nes).
Open that file on GitHub and choose **Download raw file**, then open it in Mesen.
The source build writes `build/rune-walk.nes`; refresh the copy in `dist/` when
publishing a new build.

## Build

Requirements: **cc65 V2.19 or newer**, GNU Make, and Python 3. No downloaded game
assets, npm packages, or proprietary SDKs are needed to build the ROM. All
placeholder graphics are original and generated from `tools/generate_chr.py`.

On Debian/Ubuntu, install `cc65`, `make`, and `python3` with your package manager;
on macOS, Homebrew provides `cc65`, `make`, and `python`. On Windows use those
tools in WSL, or a cc65 installation plus GNU Make and Python on PATH.

From the checkout root:

```sh
make
make check
```

The output is **`build/rune-walk.nes`**, ready to load as a ROM. `make check`
validates its size, header, vectors, and nonblank CHR ROM; it does not emulate it.
`make clean` removes generated build files. Override `PYTHON=python` if needed.

For a source installation of the exact toolchain used here:

```sh
git clone --depth 1 --branch V2.19 https://github.com/cc65/cc65.git /tmp/cc65
git -C /tmp/cc65 rev-parse HEAD
# Expected: 555282497c3ecf8b313d87d5973093af19c35bd5
make -C /tmp/cc65 -j4
make CC65_HOME=/tmp/cc65 check
```

`CC65_HOME` must point to the cc65 installation/source root containing `bin/`
and `lib/`. Otherwise the build uses `cl65` on PATH. This cloud environment
already has the tested toolchain at `/workspace/toolchains/cc65`, detected by
the Makefile. Reuse the existing checkout; a separate Git worktree is unnecessary.

## Cartridge and hardware target

| Property | Value |
| --- | --- |
| CPU/video target | Original NTSC NES, Ricoh 2A03 / 2C02 |
| Cartridge | NROM-256, mapper 0 |
| PRG ROM | 32 KiB at `$8000–$FFFF` |
| CHR ROM | 8 KiB, background and sprites use pattern table 0 |
| Mirroring | Vertical (two nametables side by side) |
| RAM | Console's internal 2 KiB only; no PRG RAM, battery, or trainer |
| File | iNES 1.0, 40,976 bytes, NTSC TV-system flag |

This uses the standard NROM configuration supported by the **EverDrive N8 Pro**.
Copy `build/rune-walk.nes` to its SD card, boot the cartridge using its installed
OS, select the ROM, and launch it. No special mapper file, save file, or expansion
audio is required. Test on an NTSC NES; PAL timing is outside this target.
Physical NES/EverDrive operation has not been verified here.

## Test in Mesen

Use a current [Mesen](https://www.mesen.ca/) desktop release (the maintained
[SourMesen/Mesen2](https://github.com/SourMesen/Mesen2) project).

1. Build the ROM, then open `build/rune-walk.nes` using Mesen's **File → Open**.
   Use NTSC region and standard speed. Configure controller port 1 as an NES
   controller and bind its D-pad. Disable cheats and rewind during timing checks.
2. Cold boot/reset: expect a grassy map and hero on the road near the left edge.
   Release all buttons: the hero stays still. Reset should restore the spawn.
3. Test every D-pad direction. Movement is one pixel per frame, with no diagonal
   movement. Horizontal input takes priority; opposite buttons on an axis cancel.
4. Walk left to the boundary and right along the road to the far boundary (about
   25 seconds). The hero stays near screen center while the map scrolls; near
   each end the camera stops and the hero approaches the edge.
5. Reverse direction repeatedly, especially near each 256-pixel screen crossing.
   Watch for stale tiles, seams, flickering, and camera jumps. Return all the way
   to the start. The map should match its first appearance.
6. Approach trees, rocks, and water from all sides. The hero's feet should stop
   against obstacles. The head can overlap them slightly: collision intentionally
   uses an inset feet box. Walk around corners to check for snagging or tunneling.
7. Open Mesen's debugger/PPU viewer. Confirm mapper 0, vertical mirroring, populated
   CHR patterns, two nametables, and four visible hero sprites. The streamed tile
   column should update ahead of the camera. With a write breakpoint on `$2007`,
   runtime tile writes should occur during vblank; boot-time writes occur with
   rendering disabled. Enable available invalid PPU access diagnostics.
8. Repeat cold boot and the traversal with randomized power-on RAM if your Mesen
   version provides that option. No state should depend on uncleared RAM.

Keep `build/rune-walk.dbg` beside the ROM for Mesen's cc65 debug-symbol loading
(or import it through the debugger). `.lbl` and `.map` are also generated. Exact
debugger menu wording varies by Mesen version. Mesen itself was not run in this
cloud environment; the checklist above is the remaining desktop acceptance test.

## Automated execution test (optional)

Node.js 22+ and JSNES 2.1.0 provide a headless regression check. The emulator is
a development test dependency only and never enters the ROM:

```sh
make check
npm install --prefix build/emulator --no-audit --no-fund --save-exact jsnes@2.1.0
node tools/smoke.mjs
```

The test boots the actual ROM, supplies controller input, verifies four-direction
movement, water/tree collision, exact movement speed, both camera clamps, a full
out-and-back traversal, and opposite-direction input. It compares every displayed
nametable tile with the map and checks runtime PPU writes against NTSC vblank.
It writes a screenshot to `build/smoke.ppm`. An alternate installed JSNES directory
can be supplied as the first argument. This supplements Mesen and physical tests.

## Code layout and extension points

- `src/startup.s`: reset, RAM initialization, interrupt vectors, OAM DMA, and
  vblank transfers. Custom startup avoids the default cc65 NES runtime's
  cartridge-RAM assumptions. NMI never calls C or touches cc65's temporary state.
- `src/video.c`, `include/nes.h`: palette, boot-time nametable fill, and a frame
  packet with one streamed column. C publishes `ready` last; NMI clears it only
  after consuming the packet. Main waits before touching it again.
- `src/world.c`: six authored 16×15-metatile screens and terrain/collision lookup.
  Each metatile is 16×16 pixels. `.`, `W`, `T`, `+`, `O`, `R` mean grass, water,
  tree, path, rock, and rune. All rows must remain exactly 16 characters long.
- `src/player.c`: world position, input interpretation, feet-box collision,
  facing, walk animation, and the four-sprite 16×16 hero.
- `src/camera.c`: bounded world-to-screen camera and circular nametable streaming.
- `src/input.c`: serial controller read. DMC is disabled, avoiding its controller
  read interference; adding sample audio will require a safe controller routine.
- `tools/generate_chr.py`: editable pixel art; emits CHR ROM deterministically.
- `cfg/nrom.cfg`: ROM/RAM budgets enforced by the linker. OAM is `$0200–$02FF`,
  game data `$0300–$05FF`, and the C software stack `$0600–$07FF`. The hardware
  stack uses `$0100–$01FF`; zero-page runtime temporaries start at `$0002`.

The map is 1536×240 pixels: horizontal scrolling is deliberate, with full
four-direction movement inside its height. The two nametables form a 512-pixel
ring. At each eight-pixel camera crossing, the next offscreen column is prepared
and uploaded during vblank. All terrain uses one background palette, so column
streaming needs no attribute changes. NMI can safely retain the old frame packet
if game logic misses a deadline. Do not teleport the camera using the streaming
path: it assumes movement of at most one tile column per update.

For towns/dungeons, add a map descriptor (dimensions, terrain data, spawn points,
and exits) behind the `world_*` API. A map transition should disable rendering,
refill both nametables, reset camera/player state, and then resume frame packets.
For combat, add a game-mode dispatcher in `main.c`, with separate exploration,
menu, and battle update/draw modules. Reuse the input and vblank packet boundary;
keep encounter rules and battle state out of the interrupt handler. Vertical
scrolling, per-terrain palettes, dialogue, and larger transfers will need an
expanded VRAM queue and a measured vblank budget.
