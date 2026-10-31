#!/usr/bin/env python3
"""Compile original scores into APU timer records; silence uses timer sentinel."""
import pathlib
import sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parents[1]/'assets'))
from music import TRACKS, CHORDS


def period(note, triangle=False):
    if not note: return (0, 0x80)  # explicit rest bit
    hz=440*2**((note-69)/12)
    n=round(1789773/(32 if triangle else 16)/hz-1)
    assert 0<=n<=2047
    return n&255,n>>8


def main():
    out=['; Generated original scores; see assets/music.py', '.segment "RODATA"']
    out+=['song_low: .lobytes '+','.join(f'song_{i}' for i in range(5)),
          'song_high: .hibytes '+','.join(f'song_{i}' for i in range(5)),
          'song_speed: .byte '+','.join(str(t[0]) for t in TRACKS),
          'song_length: .byte '+','.join(str(len(t[1])*8) for t in TRACKS)]
    for track,(tempo,bars) in enumerate(TRACKS):
        out.append(f'song_{track}:')
        for bar,melody in enumerate(bars):
            root=CHORDS[bar%16] if track!=4 else (48,53,55,48)[bar]
            minor=root in (45,50,40)
            harmony=[0,7,12,15 if minor else 16,12,7,3 if minor else 4,7]
            for step,note in enumerate(melody):
                counter=root+12+harmony[step]
                if track==2 and step%2: counter=0
                bass=root if step%4<2 else root+7
                noise=3 if step in (0,4) else 11 if step in (2,6) else 7
                if track==0 or track==2: noise=15
                values=(*period(note),*period(counter),*period(bass,True),noise)
                out.append('    .byte '+','.join(str(v) for v in values))
    pathlib.Path(sys.argv[1]).write_text('\n'.join(out)+'\n')
if __name__=='__main__': main()
