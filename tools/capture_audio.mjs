/* Select each exported APU score in the running ROM and record its real output.
 * Debug-only track selection; this is not a gameplay/progression test. */
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {boot} from '../tests/harness.mjs';
const names=['title','overworld','glassmere','battle','victory'];
const frameCounts=[1210,960,1580,700,300];
const results=[];
for(let song=0;song<names.length;song++) {
 const h=boot();h.frames(120);
 h.nes.cpu.mem[h.symbols._audio_song]=song;
 h.frames(2);h.samples.length=0;
 const beats=[];
 for(let f=0;f<frameCounts[song];f++) {h.frames(1);beats.push(h.byte('_audio_beat'));}
 const peak=h.samples.reduce((peak,v)=>Math.max(peak,Math.abs(v)),0);
 const rms=Math.sqrt(h.samples.reduce((s,v)=>s+v*v,0)/h.samples.length);
 assert(peak>0.05 && peak<0.99 && rms>0.005,'silent or clipped soundtrack');
 if(song<4) assert(beats.some((v,i)=>i && v<beats[i-1]),'score did not loop');
 else assert.equal(h.nes.papu.square1.isEnabled,false,'fanfare should end once');
 h.wav(`music-${names[song]}`);
 results.push({song:names[song],samples:h.samples.length,seconds:h.samples.length/44100,peak,rms,loop:song<4,oneShot:song===4});
 console.log(`PASS: ${names[song]} audio, playback completion, signal range, ${h.samples.length} samples`);
}
fs.writeFileSync('build/audio-results.json',JSON.stringify(results,null,2));
