# The Waking Stones — development plan

## Audit

The first build has a working NROM startup, collision, controller reads and a
horizontal two-nametable ring. It has only one background palette, sparse repeated
terrain, two-frame sprites, and no sound, title, interaction or progression. The
old headless test exercises scrolling but assumes immediate entry into that map.

## Direction and implementation order

1. Establish original art, typography and palette rules; retain NROM-256 with
   separate background/sprite pattern tables. Budget all 512 CHR tiles explicitly.
2. Add safe scene rendering and attribute-column streaming, a dedicated illustrated
   title, original title/field scores and an assembly NMI audio driver.
3. Author an eight-screen landscape with river crossings and visual regions;
   add Bracken settlement and Glassmere ruin as separate reloadable maps.
4. Add button-edge input, NPC conversation, an objective journal, recoverable
   defeat, three guardian battles, attack/defend/healing, and a chapter conclusion.
5. Execute the ROM in an emulator; check scene transitions, combat, collision,
   scroll reversals, PPU writes, audio samples and memory budgets. Inspect captures.
6. Update the downloadable ROM, documentation, quality review and next steps;
   commit and push the complete source and evidence to the connected repository.

## Scope decisions

This is an opening chapter, not a complete commercial RPG. No battery-backed
save is promised, so there is no nonfunctional Continue option. NROM stays useful
while the chapter fits 32 KiB PRG / 8 KiB CHR. Full two-axis scrolling, a large
inventory and a multi-hour campaign belong after the core presentation is proven.
