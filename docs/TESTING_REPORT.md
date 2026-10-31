# Testing report — The Waking Stones

## Tested build

- Baseline: `b92eb10`, the original movement prototype.
- Toolchain: cc65 V2.19, source commit `555282497c3ecf8b313d87d5973093af19c35bd5`.
- Runtime: JSNES 2.1.0, exact dependency and integrity recorded in `tests/package-lock.json`.
- ROM: `dist/rune-walk.nes`, 40,976 bytes, mapper 0, NTSC, vertical mirroring.
- SHA-256: `9f1a0a675f3f4e25e5371810d1ded6b023c1ff930015af7cbcd43bc20582a8a9`.

## Executed and passed

| Check | Evidence |
| --- | --- |
| Clean build | `make clean` then `make check`; all assets regenerated from committed sources |
| Reproducibility | Clean rebuild matched the prior binary SHA-256 before the final title-caption-only edit |
| Container / vectors | Exact iNES header, PRG/CHR sizes, NMI/reset/IRQ addresses and nonblank CHR |
| Static content | Tile/font/score schemas, all three maps and reachable objective/exit paths |
| Memory budget | Linker-enforced internal RAM; DATA+BSS 165 bytes, end `$03A4` |
| Actual-ROM chapter | Controller-only keeper objective, both rivers, all three guardian victories, tonics, leveling, ending and return |
| Map integrity | 4,855 displayed packets, every visible tile **and** attribute checked against independently indexed map data |
| PPU timing | 28,790 rendering-enabled VRAM writes remained in NTSC vblank; latest exploration scroll at JSNES vblank line 11/20 |
| Movement | Exact one-pixel/frame travel while streaming, both camera clamps, reverse crossings, opposite input, trees and bridge/water collision |
| State | One-time chest, defend mitigation, health arithmetic, journal, and recoverable defeat without guardian rewards |
| Power-on / input edges | Two deterministic randomized internal-RAM fixtures; held Start/A did not repeat actions; full-health tonic not consumed |
| Locked objective | Glassmere remains sealed before the keeper's conversation |
| Sound | Real APU samples for all five scores, nonzero RMS, unclipped sample range; four complete loops and one-shot fanfare completion |
| Distribution | `make release`; checked-in ROM byte-for-byte matches the tested build |

The defeat branch restores an actual emulator save-state captured before battle,
then loses by repeatedly defending. No HP, quest, enemy or inventory values are
injected to obtain a gameplay pass. The soundtrack fixture separately selects
exported song-request bytes; it does not establish gameplay progression.

## Visual and audio evidence

`docs/evidence/` contains screenshots from the actual running ROM, a contact
sheet, MP3 APU recordings and machine-readable results. PNG conversion used
Pillow; audio conversion used FFmpeg/libmp3lame. Those conversion packages are
optional evidence tools, not ROM build dependencies.

Screenshots were inspected at native and enlarged scales. Fixed defects included
long dialogue spilling outside frames, miscolored title attribute quadrants,
repeated triangular roof peaks, overbright busy dungeon flooring, and a title
caption wrapping into overscan. Continuous frame borders and a direct cursor
queue replaced weaker presentation details.

Audio was captured and analyzed for signal and playback completion. **Human
listening and mix assessment were not performed.** Non-silence is not a claim
that every arrangement is musically finished.

## Not run / remaining manual gates

Mesen desktop execution, original NTSC NES and EverDrive N8 Pro tests, television
overscan/composite palette review, physical controller feel, and human listening.
Follow `MESEN_TESTING.md`. There are no invented Mesen or hardware passes.
JSNES's timing checks are useful regression evidence, not hardware certification.

## Reproduce

```sh
make check
npm ci --prefix tests --ignore-scripts
make smoke
make audio
```

The current JSON evidence records the final branch after defeat recovery; the
chapter-completion assertions and ending screenshot precede that branch in the
same successful test run. There are no skipped or expected-failing assertions in
these scripts. Source-map/debug files remain generated under `build/`.
