# minui/makefile

.PHONY: clean

TRIMUI_VERSION=0.105

BUILD_TIME!=date +%Y%m%d
RELEASE_PATH=./release
RELEASE_BASE=MinUI-$(TRIMUI_VERSION)-$(BUILD_TIME)
RELEASE_DOT!=find release/. -name "$(RELEASE_BASE)*.zip" -printf '.' | wc -m
RELEASE_NAME=$(RELEASE_BASE)-$(RELEASE_DOT)
BUILD_PATH=./build/PAYLOAD
ROMS_PATH=./build/Roms

#--------------------------------------
all: readme sys emus tools zip
#--------------------------------------

lib:
	cd ./src/libmmenu && make

readme:
	mkdir -p "./build"
	fmt -w 40 -s "src/MinUI/readme.txt" > "./build/readme.txt"

sys: lib
	mkdir -p "$(BUILD_PATH)"
	cd ./TrimuiUpdate/ && make
	cp -R ./TrimuiUpdate/installer/. "$(BUILD_PATH)"
	cd ./src/MinUI && make
	cd ./src/encode && make
	cd ./src/show && make
	cp -R "paks/System.pak" 		"$(BUILD_PATH)"
	mkdir -p "$(BUILD_PATH)/System.pak/bin"
	mkdir -p "$(BUILD_PATH)/System.pak/lib"
	cp "src/MinUI/MinUI" 			"$(BUILD_PATH)/System.pak"
	cp "src/encode/encode" 			"$(BUILD_PATH)/System.pak/bin"
	cp "src/show/show" 				"$(BUILD_PATH)/System.pak/bin"
	cp "src/libmmenu/libmmenu.so"	"$(BUILD_PATH)/System.pak/lib"
	cp -R "paks/Update.pak" 		"$(BUILD_PATH)"

#--------------------------------------
emus: gb pm ngp gg snes ps gba nes
#--------------------------------------

emu:
	mkdir -p "$(BUILD_PATH)/Emus"

gb: emu
	mkdir -p "$(ROMS_PATH)/Game Boy"
	cd ./third-party/gambatte-dms && make -j
	cp -R "paks/Game Boy.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/gambatte-dms/gambatte-dms" "$(BUILD_PATH)/Emus/Game Boy.pak"
	
	mkdir -p "$(ROMS_PATH)/Game Boy Color"
	cp -R "paks/Game Boy Color.pak" "$(BUILD_PATH)/Emus"

pm: emu
	mkdir -p "$(ROMS_PATH)/Pokemon Mini"
	cd ./third-party/pokemini/platform/trimui && make -j
	cp -R "paks/Pokemon Mini.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/pokemini/platform/trimui/pokemini" "$(BUILD_PATH)/Emus/Pokemon Mini.pak"

ngp: emu
	mkdir -p "$(ROMS_PATH)/Neo Geo Pocket"
	cd ./third-party/race && make -j
	cp -R "paks/Neo Geo Pocket.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/race/race-od" "$(BUILD_PATH)/Emus/Neo Geo Pocket.pak"
	
	mkdir -p "$(ROMS_PATH)/Neo Geo Pocket Color"
	cp -R "paks/Neo Geo Pocket Color.pak" "$(BUILD_PATH)/Emus"

gg: emu
	mkdir -p "$(ROMS_PATH)/Game Gear"
	cd ./third-party/sms_sdl && make -j
	cp -R "paks/Game Gear.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/sms_sdl/sms_sdl" "$(BUILD_PATH)/Emus/Game Gear.pak"
	
	mkdir -p "$(ROMS_PATH)/Master System"
	cp -R "paks/Master System.pak" "$(BUILD_PATH)/Emus"

snes: emu
	mkdir -p "$(ROMS_PATH)/Super Nintendo"
	cd ./third-party/snes9x2002 && make -j
	cp -R "paks/Super Nintendo.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/snes9x2002/snes9x2002" "$(BUILD_PATH)/Emus/Super Nintendo.pak"

ps: emu
	mkdir -p "$(ROMS_PATH)/PlayStation"
	if [ ! -f ./third-party/pcsx_rearmed/config.mak ]; then cd ./third-party/pcsx_rearmed && CROSS_COMPILE=/opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi- ./configure --platform=trimui; fi
	cd ./third-party/pcsx_rearmed && make -j
	cp -R "paks/PlayStation.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/pcsx_rearmed/pcsx" "$(BUILD_PATH)/Emus/PlayStation.pak"

gba: emu
	mkdir -p "$(ROMS_PATH)/Game Boy Advance"
	cd ./third-party/gpsp-bittboy/trimui && make -j
	cp -R "paks/Game Boy Advance.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/gpsp-bittboy/trimui/gpsp" "$(BUILD_PATH)/Emus/Game Boy Advance.pak"
	cp "third-party/gpsp-bittboy/game_config.txt" "$(BUILD_PATH)/Emus/Game Boy Advance.pak"

nes: emu
	mkdir -p "$(ROMS_PATH)/Nintendo"
	cd ./third-party/fceux && make -j
	cp -R "paks/Nintendo.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/fceux/fceux/fceux.dge" "$(BUILD_PATH)/Emus/Nintendo.pak"
	cp "third-party/fceux/fceux/backdrop.png" "$(BUILD_PATH)/Emus/Nintendo.pak"
	cp "third-party/fceux/fceux/sp.bmp" "$(BUILD_PATH)/Emus/Nintendo.pak"
	cp -R "third-party/fceux/fceux/palettes" "$(BUILD_PATH)/Emus/Nintendo.pak"

#--------------------------------------
games: na
#--------------------------------------

game:
	mkdir -p "$(BUILD_PATH)/Games"

na: game
	echo "nopers"

#--------------------------------------
tools: bridge commander poweroff stock # wipe
#--------------------------------------

tool:
	mkdir -p "$(BUILD_PATH)/Tools"

bridge: tool
	cp -R "paks/USB Bridge.pak" "$(BUILD_PATH)/Tools"

commander: tool
	cd ./third-party/DinguxCommander && make -j
	cp -R "paks/Commander.pak" "$(BUILD_PATH)/Tools"
	cp "third-party/DinguxCommander/output/trimui/DinguxCommander" "$(BUILD_PATH)/Tools/Commander.pak"
	cp -R "third-party/DinguxCommander/res" "$(BUILD_PATH)/Tools/Commander.pak"

poweroff: tool
	cp -R "paks/Power Off.pak" "$(BUILD_PATH)/Tools"

stock: tool
	cp -R "paks/Stock UI.pak" "$(BUILD_PATH)/Tools"

wipe: tool
	cp -R "paks/Wipe Stock.pak" "$(BUILD_PATH)/Tools"

#--------------------------------------

zip:
	cd "$(BUILD_PATH)" && zip -r ../TrimuiUpdate_MinUI.zip . -x "*.DS_Store"
	cd ./build && zip -r ../release/$(RELEASE_NAME).zip readme.txt Roms TrimuiUpdate_MinUI.zip

#--------------------------------------

clean:
	cd ./src/libmmenu && make clean
	cd ./src/MinUI && make clean
	cd ./src/encode && make clean
	cd ./src/show && make clean
	cd ./TrimuiUpdate/ && make clean
	cd ./third-party/DinguxCommander && make clean
	cd ./third-party/gambatte-dms && make clean
	cd ./third-party/pokemini/platform/trimui && make clean
	cd ./third-party/race && make clean
	cd ./third-party/sms_sdl && make clean
	cd ./third-party/gpsp-bittboy/trimui && make clean
	cd ./third-party/snes9x2002 && make clean
	cd ./third-party/pcsx_rearmed && make clean
	rm -rf ./build