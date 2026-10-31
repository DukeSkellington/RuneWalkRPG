import assert from 'node:assert/strict';
import {boot,Controller as C} from '../tests/harness.mjs';
for(const seed of [0x35,0xc7]) {
 const h=boot();
 // A power-on fixture, before the reset vector executes; no game-state injection.
 for(let i=0;i<0x800;i++) h.nes.cpu.mem[i]=(i*73+seed)&255;
 h.frames(180);assert.equal(h.byte('_mode'),0);
 h.hold(C.BUTTON_START,60);h.frames(90);assert.equal(h.byte('_mode'),2);
 h.hold(C.BUTTON_A,120);h.frames(90);assert.equal(h.byte('_mode'),1,'held A should not reopen dialogue');
 assert.deepEqual(h.position(),[80,112]);
 h.press(C.BUTTON_START);h.frames(90);assert.equal(h.byte('_mode'),3);
 const t=h.byte('_tonics');h.press(C.BUTTON_A);h.frames(20);
 assert.equal(h.byte('_tonics'),t,'full health must not waste a tonic');
 h.press(C.BUTTON_B);h.frames(90);
 h.hold(C.BUTTON_RIGHT,1552);assert.equal(h.position()[0],1632);
 h.press(C.BUTTON_A);h.frames(90);
 assert.equal(h.byte('_mode'),2);assert.equal(h.byte('_area'),0);assert.equal(h.byte('_quest'),0);
 h.press(C.BUTTON_A);h.frames(90);assert.equal(h.byte('_mode'),1);
 console.log(`PASS: randomized startup seed ${seed}, held-button edges, full-health healing guard, sealed passage`);
}
