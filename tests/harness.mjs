import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
const packageRoot=process.env.JSNES_ROOT || 'tests/node_modules/jsnes';
export const {NES,Controller}=await import(pathToFileURL(path.resolve(packageRoot,'src/index.js')));
export function boot() {
 const symbols=Object.fromEntries(fs.readFileSync('build/rune-walk.lbl','utf8').split('\n')
 .map(s=>/^al ([0-9a-f]+) \.(\S+)/i.exec(s)).filter(Boolean).map(m=>[m[2],parseInt(m[1],16)]));
 let image; const samples=[];
 const nes=new NES({onFrame:p=>{image=p.slice();},onAudioSample:(l,r)=>samples.push((l+r)/2),sampleRate:44100});
 nes.loadROM(fs.readFileSync('build/rune-walk.nes','binary'));
 const byte=s=>nes.cpu.mem[symbols[s]];
 const word=s=>byte(s)|(nes.cpu.mem[symbols[s]+1]<<8);
 const frames=n=>{for(let i=0;i<n;i++) nes.frame();};
 const hold=(button,n)=>{nes.buttonDown(1,button);frames(n);nes.buttonUp(1,button);frames(3);};
 const press=b=>hold(b,1);
 const screenshot=name=>{
   const rgb=Buffer.alloc(256*240*3);
   image.forEach((v,i)=>{rgb[i*3]=v&255;rgb[i*3+1]=(v>>8)&255;rgb[i*3+2]=(v>>16)&255;});
   fs.writeFileSync(`build/${name}.ppm`,Buffer.concat([Buffer.from('P6\n256 240\n255\n'),rgb]));
 };
 const wav=name=>{
   const data=Buffer.alloc(44+samples.length*2);
   data.write('RIFF',0); data.writeUInt32LE(data.length-8,4);data.write('WAVEfmt ',8);
   data.writeUInt32LE(16,16);data.writeUInt16LE(1,20);data.writeUInt16LE(1,22);
   data.writeUInt32LE(44100,24);data.writeUInt32LE(88200,28);data.writeUInt16LE(2,32);data.writeUInt16LE(16,34);
   data.write('data',36);data.writeUInt32LE(samples.length*2,40);
   samples.forEach((s,i)=>data.writeInt16LE(Math.max(-32767,Math.min(32767,Math.round(s*32767))),44+i*2));
   fs.writeFileSync(`build/${name}.wav`,data);
 };
 return {nes,symbols,byte,word,frames,hold,press,screenshot,wav,samples,
 position:()=>[word('_hero'),nes.cpu.mem[symbols._hero+2]],
 state:()=>({mode:byte('_mode'),area:byte('_area'),hp:byte('_hp'),quest:byte('_quest'),guardians:byte('_guardians'),hero:[word('_hero'),nes.cpu.mem[symbols._hero+2]],camera:word('_camera_x'),pc:nes.cpu.REG_PC.toString(16)})};
}
