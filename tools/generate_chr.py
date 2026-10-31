#!/usr/bin/env python3
"""Build original editable graphics into exact NROM CHR layout. No image libraries."""
import pathlib
import sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1] / 'assets'))
from font import GLYPHS, ORDER
from terrain import TILES
from characters import FRONT, BACK, LEFT, KEEPER


def pixels(rows):
    assert all(len(row) == len(rows[0]) for row in rows)
    return [[0 if c == '.' else int(c) for c in row] for row in rows]


def tile(p, ox=0, oy=0):
    return bytes(sum(((p[oy+y][ox+x] >> plane) & 1) << (7-x) for x in range(8))
                 for plane in (0, 1) for y in range(8))


def put16(data, index, p):
    for i, (x, y) in enumerate(((0,0),(8,0),(0,8),(8,8))):
        data[(index+i)*16:(index+i+1)*16] = tile(p,x,y)


def enemy(index):
    p = [[0]*32 for _ in range(32)]
    for y in range(32):
        for x in range(32):
            dx, dy = x-15.5, y-15.5
            if index == 0:  # LANTERN MOTH, ragged amber wings
                wing = (abs(dx)-8)**2/65 + dy**2/120 < 1 and abs(dx)>2
                body = abs(dx)<2 and abs(dy)<10
                if wing: p[y][x] = 3 if (x+y)%7<2 else (2 if abs(dx)<12 else 1)
                if body: p[y][x] = 2 if y<15 else 1
                if y==5 and abs(dx)<6: p[y][x]=3
            elif index == 1:  # SHALE WISP, floating broken shell and a single eye
                if 4 < abs(dx)+abs(dy) < 18 and -10<dy<9:
                    p[y][x] = 3 if dx+dy < -4 else (2 if dx<3 else 1)
                if abs(dx)<5 and abs(dy)<2: p[y][x]=3
                if abs(dx)<2 and abs(dy)<2: p[y][x]=1
                if y>25 and (x+y)%11<3 and 8<x<25: p[y][x]=2
            else:  # HOLLOW SENTINEL, horned guardian, split armor
                if 7<x<24 and 7<y<27:
                    p[y][x] = 3 if x<12 else (2 if x<21 else 1)
                    if (x==15 or y==19): p[y][x]=1
                if 8<x<23 and 8<y<14: p[y][x]=1
                if y==11 and x in (11,12,19,20): p[y][x]=3
                if 2<y<10 and abs(dx) in (7.5,8.5,9.5): p[y][x]=2
                if y>26 and x in range(8,13): p[y][x]=1
                if y>26 and x in range(19,24): p[y][x]=1
                if 9<y<27 and x in (4,5): p[y][x]=3
    return p


def main():
    data = bytearray(8192)
    assert len(TILES)==24 and len(ORDER)<=48
    for n, rows in enumerate(TILES):
        assert len(rows)==16 and all(len(r)==16 for r in rows), n
        put16(data,n*4,pixels(rows))
    for n, ch in enumerate(ORDER):
        p = [[0]*8 for _ in range(8)]
        for y,row in enumerate(GLYPHS[ch]):
            for x,c in enumerate(row): p[y][x+1] = 3 if c=='1' else 0
        data[(96+n)*16:(97+n)*16]=tile(p)
    # Bespoke beveled logo, 21x3 tiles. Gold top edge, copper undersides.
    logo = [[0]*168 for _ in range(24)]
    mask = [[False]*168 for _ in range(24)]
    xpos=4
    for ch in 'RUNE WALK':
        if ch==' ': xpos+=9; continue
        for y,row in enumerate(GLYPHS[ch]):
            for x,c in enumerate(row):
                if c=='1':
                    for sy in range(3):
                        for sx in range(3): mask[y*3+1+sy][xpos+x*3+sx]=True
        xpos+=18
    for y in range(23):
        for x in range(167):
            if mask[y][x]:
                logo[y+1][x+1]=1
    for y in range(24):
        for x in range(168):
            if mask[y][x]: logo[y][x]=3 if y==0 or not mask[y-1][x] else 2
    for y in range(3):
        for x in range(21):
            i=144+y*21+x
            data[i*16:(i+1)*16]=tile(logo,x*8,y*8)
    for n in range(3):
        p=enemy(n)
        for y in range(4):
            for x in range(4):
                i=208+n*16+y*4+x
                data[i*16:(i+1)*16]=tile(p,x*8,y*8)
    # Pattern table 1: sixteen traveler poses, keeper, and sparkle.
    for direction,rows in enumerate((FRONT,BACK,LEFT,[r[::-1] for r in LEFT])):
        for frame in range(4):
            p=pixels(rows)
            if frame:
                # Standing is distinct. Three walking phases alternate boots,
                # cloak hem and hands; the full silhouette remains 16x16.
                for y in range(13,16): p[y]=[0]*16
                a,b=((3,10),(5,9),(4,11))[frame-1]
                for y in range(13,16):
                    for x in (a,a+1,b,b+1): p[y][x]=2 if y==13 else 1
                if frame in (1,3):
                    p[8][2]=0; p[10][12]=3
            put16(data,256+direction*16+frame*4,p)
    put16(data,320,pixels(KEEPER))
    for phase in range(4):
        p=[[0]*8 for _ in range(8)]
        for y in range(8):
            for x in range(8):
                if (x==3 or y==3) and abs(x-3)+abs(y-3)<=phase+1: p[y][x]=3
        data[(324+phase)*16:(325+phase)*16]=tile(p)
    # Dedicated border patterns in the two remaining background slots.
    vertical=[[2 if x==3 else 0 for x in range(8)] for y in range(8)]
    horizontal=[[2 if y==3 else 0 for x in range(8)] for y in range(8)]
    data[143*16:144*16]=tile(vertical)
    data[207*16:208*16]=tile(horizontal)
    moon=[[0]*16 for _ in range(16)]
    for y in range(16):
        for x in range(16):
            if (x-7.5)**2+(y-7.5)**2<52:
                moon[y][x]=3 if x<10 else 2
                if (x,y) in ((4,5),(5,5),(5,6),(8,10),(9,10)): moon[y][x]=2
    put16(data,328,moon)
    pathlib.Path(sys.argv[1]).write_bytes(data)

if __name__=='__main__': main()
