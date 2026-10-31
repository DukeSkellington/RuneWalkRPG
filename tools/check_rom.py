#!/usr/bin/env python3
"""Check the cartridge container and vectors; execution needs an emulator."""
import pathlib
import struct
import sys

rom = pathlib.Path(sys.argv[1]).read_bytes()
assert len(rom) == 16 + 32768 + 8192, f"Unexpected ROM size: {len(rom)}"
assert rom[:16] == b"NES\x1a\x02\x01\x01" + bytes(9), "Incorrect NROM/NTSC header"
vectors = struct.unpack_from("<HHH", rom, 16 + 32768 - 6)
assert all(0x8000 <= v < 0xFFFA for v in vectors), f"Invalid vectors: {vectors}"
assert len(set(rom[16 + 32768:])) > 8, "CHR ROM appears blank"
print("PASS: 40,976 bytes; mapper 0; 32 KiB PRG; 8 KiB CHR; vertical mirroring; NTSC")
print("PASS: NMI/reset/IRQ vectors point into PRG ROM; nonblank CHR data")
