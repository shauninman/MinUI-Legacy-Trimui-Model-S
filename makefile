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
	cp "readme.md" "./build"
	

sys: lib
	mkdir -p "$(BUILD_PATH)"
	cd ./TrimuiUpdate/ && make
	cp -R ./TrimuiUpdate/installer/. "$(BUILD_PATH)"
	cd ./src/ui && make
	cd ./src/encode && make
	cd ./src/show && make
	cp -R "paks/System.pak" 		"$(BUILD_PATH)"
	cp "src/ui/MinUI" 				"$(BUILD_PATH)/System.pak"
	cp "src/encode/encode" 			"$(BUILD_PATH)/System.pak"
	cp "src/show/show" 				"$(BUILD_PATH)/System.pak"
	cp "src/libmmenu/libmmenu.so"	"$(BUILD_PATH)/System.pak/lib"
	cp -R "paks/Update.pak" 		"$(BUILD_PATH)"

#--------------------------------------
emus: gb pm ngp gg gba sms
#--------------------------------------

emu:
	mkdir -p "$(BUILD_PATH)/Emus"

gb: emu
	mkdir -p "$(ROMS_PATH)/Game Boy"
	cd ./third-party/gambatte-dms && make
	cp -R "paks/Game Boy.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/gambatte-dms/gambatte-dms" "$(BUILD_PATH)/Emus/Game Boy.pak"
	
	mkdir -p "$(ROMS_PATH)/Game Boy Color"
	cp -R "paks/Game Boy Color.pak" "$(BUILD_PATH)/Emus"

pm: emu
	mkdir -p "$(ROMS_PATH)/Pokemon Mini"
	cd ./third-party/pokemini/platform/trimui && make
	cp -R "paks/Pokemon Mini.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/pokemini/platform/trimui/pokemini" "$(BUILD_PATH)/Emus/Pokemon Mini.pak"

ngp: emu
	mkdir -p "$(ROMS_PATH)/Neo Geo Pocket"
	cd ./third-party/race && make
	cp -R "paks/Neo Geo Pocket.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/race/race-od" "$(BUILD_PATH)/Emus/Neo Geo Pocket.pak"
	
	mkdir -p "$(ROMS_PATH)/Neo Geo Pocket Color"
	cp -R "paks/Neo Geo Pocket Color.pak" "$(BUILD_PATH)/Emus"

gg: emu
	mkdir -p "$(ROMS_PATH)/Game Gear"
	cd ./third-party/sms_sdl && make
	cp -R "paks/Game Gear.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/sms_sdl/sms_sdl" "$(BUILD_PATH)/Emus/Game Gear.pak"

gba: emu
	mkdir -p "$(ROMS_PATH)/Game Boy Advance"
	cd ./third-party/gpsp-bittboy/trimui && make
	cp -R "paks/Game Boy Advance.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/gpsp-bittboy/trimui/gpsp" "$(BUILD_PATH)/Emus/Game Boy Advance.pak"
	cp "third-party/gpsp-bittboy/game_config.txt" "$(BUILD_PATH)/Emus/Game Boy Advance.pak"

sms: emu
	mkdir -p "$(ROMS_PATH)/Master System"
	cd ./third-party/sms_sdl && make
	cp -R "paks/Master System.pak" "$(BUILD_PATH)/Emus"
	cp "third-party/sms_sdl/sms_sdl" "$(BUILD_PATH)/Emus/Master System.pak"

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
	cd ./third-party/DinguxCommander && make
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
	cd ./build && zip -r ../release/$(RELEASE_NAME).zip readme.md Roms TrimuiUpdate_MinUI.zip

#--------------------------------------

clean:
	cd ./src/libmmenu && make clean
	cd ./src/ui && make clean
	cd ./src/encode && make clean
	cd ./src/show && make clean
	cd ./TrimuiUpdate/ && make clean
	cd ./third-party/DinguxCommander && make clean
	cd ./third-party/gambatte-dms && make clean
	cd ./third-party/pokemini/platform/trimui && make clean
	cd ./third-party/race && make clean
	cd ./third-party/sms_sdl && make clean
	cd ./third-party/gpsp-bittboy/trimui && make clean
	rm -rf ./build