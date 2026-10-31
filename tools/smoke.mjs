/* Execute the real ROM in JSNES 2.1.0 (optional Node.js test dependency).
 * Usage: node tools/smoke.mjs /path/to/node_modules/jsnes
 * No game state is injected: movement uses controller port 1 throughout. */
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
const root = path.resolve(process.argv[2] || 'build/emulator/node_modules/jsnes');
const {NES, Controller} = await import(pathToFileURL(path.join(root, 'src/index.js')));
const labels = Object.fromEntries(fs.readFileSync('build/rune-walk.lbl', 'utf8')
  .split('\n').map(line => /^al ([0-9A-F]+) \.(\S+)/i.exec(line))
  .filter(Boolean).map(m => [m[2], parseInt(m[1], 16)]));
const rows = [...fs.readFileSync('src/world.c', 'utf8').matchAll(/"([.TW+OR]{16})"/g)].map(m => m[1]);
assert.equal(rows.length, 90);
const kind = {'.': 0, W: 1, T: 2, '+': 3, O: 4, R: 5};
function expectedTile(x, y) {
  const cellX = x >> 1;
  const c = cellX >= 96 ? 'T' : rows[(cellX >> 4) * 15 + (y >> 1)][cellX & 15];
  return kind[c] * 4 + ((y & 1) << 1) + (x & 1);
}
let framebuffer;
const nes = new NES({onFrame: pixels => {framebuffer = pixels.slice();}, onAudioSample() {}});
nes.loadROM(fs.readFileSync('build/rune-walk.nes', 'binary'));
const byte = name => nes.cpu.mem[labels[name]];
const word = name => byte(name) | (nes.cpu.mem[labels[name] + 1] << 8);
const position = () => [word('_hero'), nes.cpu.mem[labels._hero + 2]];
let checkedFrames = 0, writes = 0, maxVblankLine = 0;
const originalScroll = nes.ppu.scrollWrite.bind(nes.ppu);
nes.ppu.scrollWrite = function(value) {
  const second = !this.firstWrite;
  originalScroll(value);
  if (!second || !this.f_bgVisibility) return;
  assert(this.scanline < 20, 'scroll update exceeded NTSC vblank');
  maxVblankLine = Math.max(maxVblankLine, this.scanline);
  const camera = word('_camera_x');
  assert.equal(this.regHT * 8 + this.regFH + this.regH * 256, camera & 511);
  const first = camera >> 3;
  const last = (camera + 255) >> 3;
  for (let x = first; x <= last; x++) {
    const physical = x & 63;
    const base = physical & 32 ? 0x2400 : 0x2000;
    for (let y = 0; y < 30; y++) {
      assert.equal(this.vramMem[base + y * 32 + (physical & 31)], expectedTile(x, y),
        `stale visible tile: camera=${camera}, world tile=${x},${y}`);
    }
  }
  checkedFrames++;
};
const originalVRAM = nes.ppu.vramWrite.bind(nes.ppu);
nes.ppu.vramWrite = function(value) {
  if (this.f_bgVisibility) {
    assert(this.scanline < 20, 'VRAM write outside NTSC vblank');
    writes++;
  }
  originalVRAM(value);
};
function frames(count) {for (let i = 0; i < count; i++) nes.frame();}
function hold(button, count) {
  nes.buttonDown(1, button); frames(count); nes.buttonUp(1, button); frames(2);
}
frames(120);
assert.deepEqual(position(), [48, 112], 'boot position');
assert.equal(nes.ppu.f_bgVisibility, 1);
assert.equal(nes.ppu.f_spVisibility, 1);
hold(Controller.BUTTON_LEFT, 16); // clear the conifers above the spawn point
hold(Controller.BUTTON_UP, 300);
assert.equal(position()[1], 8, 'top tree collision');
hold(Controller.BUTTON_DOWN, 104);
assert.equal(position()[1], 112, 'downward movement');
hold(Controller.BUTTON_LEFT, 100);
assert.equal(position()[0], 14, 'left tree collision');
assert.equal(word('_camera_x'), 0, 'left camera clamp');
// Water occupies x=144..191, y=48..95. Approach its lower edge.
hold(Controller.BUTTON_RIGHT, 146);
assert.equal(position()[0], 160);
hold(Controller.BUTTON_UP, 80);
assert.equal(position()[1], 88, 'water collision');
hold(Controller.BUTTON_DOWN, 24);
assert.equal(position()[1], 112);
const startX = position()[0];
hold(Controller.BUTTON_RIGHT, 400);
assert.equal(position()[0], startX + 400, 'one pixel per NTSC frame including streaming');
hold(Controller.BUTTON_RIGHT, 1300);
assert.equal(position()[0], 1506, 'right boundary collision');
assert.equal(word('_camera_x'), 1280, 'right camera clamp');
hold(Controller.BUTTON_LEFT, 1700);
assert.equal(position()[0], 14, 'return trip across ring-buffer seams');
assert.equal(word('_camera_x'), 0);
// Simultaneous horizontal directions must cancel.
nes.buttonDown(1, Controller.BUTTON_RIGHT);
nes.buttonDown(1, Controller.BUTTON_LEFT);
frames(20);
assert.equal(position()[0], 14);
nes.buttonUp(1, Controller.BUTTON_RIGHT);
nes.buttonUp(1, Controller.BUTTON_LEFT);
hold(Controller.BUTTON_RIGHT, 430);
assert(writes > 1000 && checkedFrames > 3000, 'streaming checks actually ran');
// Portable screenshot, useful in CI without a browser or display server.
const rgb = Buffer.alloc(256 * 240 * 3);
for (let i = 0; i < framebuffer.length; i++) {
  rgb[i * 3] = framebuffer[i] & 255;
  rgb[i * 3 + 1] = (framebuffer[i] >> 8) & 255;
  rgb[i * 3 + 2] = (framebuffer[i] >> 16) & 255;
}
fs.writeFileSync('build/smoke.ppm', Buffer.concat([Buffer.from('P6\n256 240\n255\n'), rgb]));
console.log(`PASS: boot, four directions, collision, camera clamps, full return trip, opposite inputs`);
console.log(`PASS: ${checkedFrames} displayed packets checked against map; ${writes} streamed tile writes`);
console.log(`PASS: VRAM/scroll updates inside NTSC vblank (latest scroll: line ${maxVblankLine} of 20)`);
