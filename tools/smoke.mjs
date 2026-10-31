/* Actual-ROM regression playthrough. JSNES_ROOT selects a JSNES 2.1.0 install. */
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {boot,Controller as C} from '../tests/harness.mjs';
const h=boot(); const {nes,byte,word,frames,hold,press,position}=h;
const maps=JSON.parse(fs.readFileSync('build/world.json','utf8'));
const palettes=[0,0,0,0,0,1,1,2,3,3,3,1,3,3,1,3,1,1,3,1,1,1,3,0];
let packets=0,writes=0,latestLine=0;
const scroll=nes.ppu.scrollWrite.bind(nes.ppu);
nes.ppu.scrollWrite=function(v) {
 const second=!this.firstWrite; scroll(v);
 if(!second || byte('_loading') || byte('_render_mask')!==0x1e || byte('_mode')!==1) return;
 assert(this.scanline<20,'scroll overran NTSC vblank'); latestLine=Math.max(latestLine,this.scanline);
 const camera=word('_camera_x'),map=maps[byte('_area')];
 assert.equal(this.regHT*8+this.regFH+this.regH*256,camera&511);
 for(let x=camera>>3;x<=((camera+255)>>3);x++) {
  const p=x&63,base=p&32?0x2400:0x2000;
  for(let y=0;y<30;y++) {
   const id=map[y>>1][x>>1];
   const expected=id*4+((y&1)<<1)+(x&1);
   assert.equal(this.vramMem[base+y*32+(p&31)],expected,`tile camera=${camera}, area=${byte('_area')}, x=${x}, y=${y}`);
   const attr=this.vramMem[base+0x3c0+(y>>2)*8+((p&31)>>2)];
   const shift=((y&2)?4:0)+((x&2)?2:0);
   assert.equal((attr>>shift)&3,palettes[id],`attribute camera=${camera}, x=${x}, y=${y}`);
  }
 }
 packets++;
};
const write=nes.ppu.vramWrite.bind(nes.ppu);
nes.ppu.vramWrite=function(v) {
 if(this.f_bgVisibility) { assert(this.scanline<20,'rendering-time VRAM write'); writes++; }
 write(v);
};
function settle() {frames(90);}
function tap(button) {press(button);settle();}
function shot(n) {h.screenshot(n);}
function atMode(m) {assert.equal(byte('_mode'),m,JSON.stringify(h.state()));}
frames(180);atMode(0);shot('title');
assert.equal(byte('_audio_current'),0);assert(h.samples.some(v=>Math.abs(v)>0.05),'title audio is silent');
assert(nes.papu.square1.isEnabled && nes.papu.square2.isEnabled && nes.papu.triangle.isEnabled);
tap(C.BUTTON_START);atMode(2);shot('intro');tap(C.BUTTON_A);atMode(1);shot('field');
// One pixel/frame and collision, before accepting the chapter objective.
hold(C.BUTTON_LEFT,200);assert.equal(position()[0],14);
hold(C.BUTTON_RIGHT,178);assert.equal(position()[0],192);
tap(C.BUTTON_A);assert.equal(byte('_area'),1);atMode(1);shot('town');
hold(C.BUTTON_RIGHT,128);assert.equal(position()[0],176);
tap(C.BUTTON_A);atMode(2);assert.equal(byte('_quest'),1);shot('keeper');tap(C.BUTTON_B);
hold(C.BUTTON_LEFT,160);assert.equal(position()[0],16);tap(C.BUTTON_A);
assert.equal(byte('_area'),0);assert.equal(position()[0],208);
// Opposite directions cancel without introducing diagonal speed.
const before=position();
nes.buttonDown(1,C.BUTTON_LEFT); nes.buttonDown(1,C.BUTTON_RIGHT); frames(20);
nes.buttonUp(1,C.BUTTON_LEFT); nes.buttonUp(1,C.BUTTON_RIGHT); frames(3);
assert.deepEqual(position(),before);
// River crossing and scroll seam reversals, with exact fixed-speed movement.
hold(C.BUTTON_RIGHT,420);assert.equal(position()[0],628);shot('river');
hold(C.BUTTON_RIGHT,40);assert.equal(position()[0],668);
hold(C.BUTTON_UP,40);assert.equal(position()[1],104,'bridge keeps feet out of the river');
hold(C.BUTTON_DOWN,40); // opposite river bank clamps the feet back to the road
while(position()[1]>112) hold(C.BUTTON_UP,1);
while(position()[1]<112) hold(C.BUTTON_DOWN,1);
hold(C.BUTTON_RIGHT,500);assert.equal(position()[0],1168);shot('highlands');
hold(C.BUTTON_LEFT,400);assert.equal(position()[0],768);
hold(C.BUTTON_RIGHT,864);assert.equal(position()[0],1632);
hold(C.BUTTON_RIGHT,500);assert.equal(position()[0],2018);assert.equal(word('_camera_x'),1792);
shot('field-east');hold(C.BUTTON_LEFT,386);assert.equal(position()[0],1632);
tap(C.BUTTON_A);atMode(1);assert.equal(byte('_area'),2);shot('ruin');
// Approach sleeping rune; interaction from the road starts a fixed guardian fight.
hold(C.BUTTON_RIGHT,64);hold(C.BUTTON_UP,40);assert.equal(position()[1],88);
const beforeChest=byte('_tonics');tap(C.BUTTON_A);assert.equal(byte('_tonics'),beforeChest+1);tap(C.BUTTON_B);
tap(C.BUTTON_A);assert.equal(byte('_tonics'),beforeChest+1,'chest can only be collected once');tap(C.BUTTON_B);
hold(C.BUTTON_DOWN,24);hold(C.BUTTON_RIGHT,240);assert.equal(position()[0],352);
tap(C.BUTTON_A);atMode(4);assert.equal(byte('_enemy'),0);shot('battle');
const battleCheckpoint=JSON.parse(JSON.stringify(nes.toJSON()));
const hp0=byte('_hp');tap(C.BUTTON_B);assert.equal(byte('_hp'),hp0-1,'defend mitigates damage');
assert.equal(byte('_combat_phase'),1);tap(C.BUTTON_A);
// Finish all three battles using real menu input, healing as needed.
for(let foe=0;foe<3;foe++) {
 assert.equal(byte('_enemy'),foe);
 let turns=0;
 while(byte('_combat_phase')!==2) {
  assert(++turns<15,'battle did not resolve');
  if(byte('_combat_phase')===1) {tap(C.BUTTON_A);continue;}
  // At low health, select TONIC. Otherwise select ATTACK.
  const desired=byte('_hp')<12 && byte('_tonics')?2:0;
  while(byte('_combat_selection')!==desired) tap(C.BUTTON_DOWN);
  tap(C.BUTTON_A);
  assert.notEqual(byte('_combat_phase'),3,'playthrough unexpectedly defeated');
 }
 shot(`victory-${foe}`);tap(C.BUTTON_A);atMode(1);
 assert.equal(byte('_guardians'),foe+1);
 if(foe<2) tap(C.BUTTON_A);
}
assert(byte('_level')>=2,'experience did not advance level');
tap(C.BUTTON_A);atMode(5);assert.equal(byte('_quest'),2);shot('ending');tap(C.BUTTON_A);
assert.equal(byte('_area'),1);tap(C.BUTTON_START);atMode(3);shot('journal');tap(C.BUTTON_B);atMode(1);
// Restore a real emulator checkpoint (not injected game variables), then lose
// by defending repeatedly. No reward or quest completion should be granted.
nes.fromJSON(battleCheckpoint);frames(3);
let defeatTurns=0;
while(byte('_combat_phase')!==3) {
 assert(++defeatTurns<60,'defeat did not resolve');
 if(byte('_combat_phase')===1) tap(C.BUTTON_A);
 else tap(C.BUTTON_B);
}
assert.equal(byte('_hp'),0);shot('defeat');tap(C.BUTTON_A);
assert.equal(byte('_area'),1);assert.equal(byte('_hp'),byte('_max_hp'));
assert.equal(byte('_guardians'),0);atMode(1);
assert(packets>3000 && writes>10000,'timing/streaming checks did not execute');
console.log(`PASS: complete chapter by controller input, keeper objective, 3 enemies, defend, tonics, leveling, ending, journal, one-time chest, defeat recovery`);
console.log(`PASS: ${packets} map packets: every visible tile AND attribute; ${writes} vblank VRAM writes; latest scroll line ${latestLine}/20`);
fs.writeFileSync('build/test-results.json',JSON.stringify({packets,writes,latestLine,state:h.state()},null,2));
