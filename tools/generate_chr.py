#!/usr/bin/env python3
"""Original Rune Walk pixel art. No external assets; emit exactly 8 KiB CHR."""
import pathlib
import sys


def terrain(kind):
    p = [[1 for _ in range(16)] for _ in range(16)]
    for y in range(16):
        for x in range(16):
            if kind == 0:  # grass tufts
                p[y][x] = 3 if (x + 3 * y) % 17 == 0 else 1
            elif kind == 1:  # blue water, pale ripples
                p[y][x] = 3 if y % 5 == 0 and (x + y) % 8 < 5 else 2
            elif kind == 2:  # conifer
                crown = 2 <= y <= 12 and abs(x - 7) <= y // 2
                p[y][x] = (1 if (x + y) % 3 else 0) if crown else 1
                if y > 11 and 6 <= x <= 9:
                    p[y][x] = 3
            elif kind == 3:  # cobbled road
                p[y][x] = 3 if y % 8 and (x + (4 if y > 7 else 0)) % 8 else 1
            elif kind == 4:  # boulder
                if 3 <= y <= 13 and 2 <= x <= 13 and 5 <= x + y <= 25:
                    p[y][x] = 3 if x + y < 14 else 0
            elif kind == 5:  # rune stone
                if 2 <= x <= 13 and 1 <= y <= 14:
                    p[y][x] = 0
                if (x == 7 and 4 <= y <= 11) or (y == 5 and 5 <= x <= 10) or (x - y == 2 and 5 <= y <= 8):
                    p[y][x] = 3
    return p


def hero(facing, frame):
    # 0 transparent, 1 red cloak, 2 gold hair/boots, 3 light face.
    p = [[0] * 16 for _ in range(16)]
    for y in range(1, 15):
        for x in range(4, 12):
            if y < 4:
                p[y][x] = 2
            elif y < 7:
                p[y][x] = 2 if facing == 1 else 3
            elif y < 12:
                p[y][x] = 1
            elif x in (4 + frame, 5 + frame, 10 - frame, 11 - frame):
                p[y][x] = 2
    if facing in (2, 3):
        eye = 4 if facing == 2 else 11
        p[4][eye] = 0
        p[8][3 if facing == 2 else 12] = 3
    elif facing == 0:
        p[4][5] = p[4][10] = 0
    return p


def tiles(pixels):
    out = bytearray()
    for oy, ox in ((0, 0), (0, 8), (8, 0), (8, 8)):
        for plane in (0, 1):
            for y in range(oy, oy + 8):
                out.append(sum(((pixels[y][ox + x] >> plane) & 1) << (7 - x) for x in range(8)))
    return out


def main():
    data = bytearray(8192)
    for kind in range(6):
        data[kind * 64:kind * 64 + 64] = tiles(terrain(kind))
    for facing in range(4):
        for frame in range(2):
            offset = (32 + facing * 8 + frame * 4) * 16
            data[offset:offset + 64] = tiles(hero(facing, frame))
    pathlib.Path(sys.argv[1]).write_bytes(data)


if __name__ == "__main__":
    main()
