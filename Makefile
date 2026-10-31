# cc65 V2.19 or newer; override CC65_HOME for a local source installation.
CC65_HOME ?= /workspace/toolchains/cc65
ifneq ($(wildcard $(CC65_HOME)/bin/cl65),)
CL65 := $(CC65_HOME)/bin/cl65
export CC65_HOME
else
CL65 := cl65
endif
PYTHON ?= python3
C_SOURCES := $(wildcard src/*.c)
ASM_SOURCES := src/startup.s src/chr.s src/audio.s
OBJECTS := $(patsubst src/%.c,build/%.o,$(C_SOURCES)) $(patsubst src/%.s,build/%.o,$(ASM_SOURCES))

.PHONY: all clean check smoke audio release
all: build/rune-walk.nes
build:
	mkdir -p build
build/tiles.chr: tools/generate_chr.py $(wildcard assets/*.py) | build
	$(PYTHON) $< $@
build/world.inc: tools/generate_world.py assets/terrain.py | build
	$(PYTHON) $< $@
build/music.inc: tools/generate_music.py assets/music.py | build
	$(PYTHON) $< $@
build/%.o: src/%.c $(wildcard include/*.h) build/world.inc | build
	$(CL65) -t nes -Oirs -g -I include -I . -c -o $@ $<
build/%.o: src/%.s build/tiles.chr build/music.inc | build
	$(CL65) -t nes -g --asm-include-dir . -c -o $@ $<
build/rune-walk.nes: $(OBJECTS) cfg/nrom.cfg
	$(CL65) -t nes -C cfg/nrom.cfg -m build/rune-walk.map -Ln build/rune-walk.lbl -Wl --dbgfile,build/rune-walk.dbg -o $@ $(OBJECTS)
check: build/rune-walk.nes
	$(PYTHON) tools/check_rom.py $<
	$(PYTHON) tools/check_assets.py
smoke: check
	node tools/smoke.mjs
	node tools/edge_cases.mjs
audio: check
	node tools/capture_audio.mjs
release: check
	cp build/rune-walk.nes dist/rune-walk.nes
clean:
	rm -rf build
