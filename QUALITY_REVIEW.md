# Quality review — The Waking Stones

## Strongest parts

The build is now an actual introductory RPG chapter rather than a movement demo.
A title, a place to arrive, someone to speak to, a journey, three encounters and a
conclusion give the mechanics context. Defeat is recoverable, the chest is finite,
and the journal explains the current objective.

The art has a coherent copper / forest / moonlit-stone vocabulary, editable
original metatiles, a recognizable scarf-and-cloak traveler and original lettering.
Proper per-metatile palettes make a substantial difference over the single-palette
prototype. Roof slopes span buildings rather than repeating little peaks.
The title uses an illustrated composition, not just text on black.

The technical foundation is restrained and testable. It stays on NROM, uses only
internal NES RAM, and verifies tile and attribute ring updates by executing the
ROM. Audio has an independent assembly driver and original scored material.
Palette fades and a small cursor queue stay inside the same vblank contract.

## Weakest parts and honest assessment

This does **not yet reach the visual density, animation variety or musical finish
of an exceptional late-generation commercial NES RPG**. It is a substantial,
cohesive opening chapter and a sound base for that goal, not the finished goal.

- The landscape still has long horizontal stretches, regular forest masses and
  hard terrain boundaries. It needs more deliberate shore/cliff transitions,
  organic forest edges, local secrets and landmarks visible before arrival.
- Bracken has one speaking NPC and no building interiors. Glassmere is a small
  columned hall, not a developed dungeon with spatial puzzles.
- Enemy portraits are 32×32 and battles are deterministic. They need animation,
  impact presentation, clearer telegraphing and more distinct tactical choices.
- The traveler has all required poses, but the 16×16 silhouette and walk timing
  deserve more pixel-by-pixel animation work. There are no autonomous NPCs.
- Dialogue and battle messages use full-screen transitions. The cursor is quick,
  but an in-world lower message window would make conversation less interruptive.
- The title and field have complete musical forms; the ruin and battle tracks
  arrange those motifs rather than offering equally developed independent themes.
  Sound effects are a small noise-channel vocabulary. Human listening and an
  experienced chiptune arranger's mix review remain necessary.
- There is no save system, inventory equipment, economy, text paging, accessibility
  settings or complete game campaign. No fake Continue option is shown.

## Verification boundary

Actual-ROM controller playthroughs, reverse scrolling, pixel/attribute comparisons,
PPU write timing, health/progression/recovery, audio signal and playback completion
checks passed. Screenshots were inspected and corrected for text overflow,
incorrect title attributes, busy dungeon flooring and roof repetition.

Mesen desktop, original NES/EverDrive execution, controller feel on physical pads,
composite-TV palette/overscan and human soundtrack listening remain manual gates.
No claim of physical-hardware certification or commercial release readiness is made.
