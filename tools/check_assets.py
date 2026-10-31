#!/usr/bin/env python3
"""Validate cartridge budgets and meaningful static adventure invariants."""
from collections import deque
import json
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'assets'))
from terrain import TILES,PALETTES,SOLID
from font import ORDER,GLYPHS
from music import TRACKS
assert len(TILES)==24 and len(PALETTES)==24
assert len(set(ORDER))==len(ORDER)<=47
for tile in TILES:
    assert len(tile)==16 and all(len(r)==16 and set(r)<=set('.123') for r in tile)
for ch in ORDER:
    assert len(GLYPHS[ch])==7 and all(len(r)==5 and set(r)<=set('01') for r in GLYPHS[ch])
for tempo,bars in TRACKS:
    assert tempo>=5 and len(bars)*8<=128
    assert all(len(bar)==8 for bar in bars)
maps=json.loads(Path('build/world.json').read_text())
for i,m in enumerate(maps):
    width=(128,32,32)[i]
    assert len(m)==15 and all(len(r)==width for r in m)
    assert all(0<=v<24 for row in m for v in row)
    # A route must exist from each entry to its actual objective/exit.
    start=(5 if i==0 else 3,7)
    destinations=([(12,7),(102,7),(126,7)],[(1,7),(11,7)],[(1,7),(22,7),(7,6)])[i]
    seen={start}; todo=deque([start])
    while todo:
        x,y=todo.popleft()
        for a,b in ((x-1,y),(x+1,y),(x,y-1),(x,y+1)):
            if 0<=a<width and 0<=b<15 and m[b][a] not in SOLID and (a,b) not in seen:
                seen.add((a,b));todo.append((a,b))
    assert all(d in seen for d in destinations),(i,destinations)
# Labels come from this link and enforce the real internal-RAM allocation.
labels={}
for row in Path('build/rune-walk.lbl').read_text().splitlines():
    fields=row.split()
    if len(fields)==3: labels[fields[2].lstrip('.')]=int(fields[1],16)
assert labels['__BSS_RUN__']+labels['__BSS_SIZE__']<=0x600
assert labels['__DATA_RUN__']+labels['__DATA_SIZE__']<=0x600
assert len(Path('build/tiles.chr').read_bytes())==8192
print('PASS: original asset schema, CHR allocation, all map objective routes, internal RAM budget')
