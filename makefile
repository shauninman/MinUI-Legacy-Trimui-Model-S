.PHONY: clean

TRIMUI_VERSION=0.105

BUILD_TIME!=date +%Y%m%d
RELEASE_BASE=MinUI-$(TRIMUI_VERSION)-$(BUILD_TIME)
RELEASE_DOT!=find release/. -name "$(RELEASE_BASE)*.zip" -printf '.' | wc -m
RELEASE_NAME=$(RELEASE_BASE)-$(RELEASE_DOT)
BUILD_PATH=build/$(RELEASE_NAME)

all: base extras trimui readme zip
base: system tools emus

zip:
	mkdir -p "release"
	cd "build" && zip -r "$(RELEASE_NAME).zip" "$(RELEASE_NAME)"
	mv "$(BUILD_PATH).zip" "release"

# MINUI

system:
	mkdir -p "$(BUILD_PATH)/SDCARD"
	cd ./src/ui && make
	cd ./src/encode && make
	cd ./src/show && make
	cp -R "paks/System.pak" "$(BUILD_PATH)/SDCARD"
	cp "src/ui/MinUI" 		"$(BUILD_PATH)/SDCARD/System.pak"
	cp "src/encode/encode" 	"$(BUILD_PATH)/SDCARD/System.pak"
	cp "src/show/show" 		"$(BUILD_PATH)/SDCARD/System.pak"

readme:
	mkdir -p "$(BUILD_PATH)"
	cp "readme.md" "$(BUILD_PATH)"

# TOOLS

tools: adb commander poweroff

adb:
	mkdir -p "$(BUILD_PATH)/SDCARD/Tools"
	cp -R "paks/USB Bridge.pak" "$(BUILD_PATH)/SDCARD/Tools"

commander:
	mkdir -p "$(BUILD_PATH)/SDCARD/Tools"
	cd ./third-party/DinguxCommander && make
	cp -R "paks/Commander.pak" "$(BUILD_PATH)/SDCARD/Tools"
	cp "third-party/DinguxCommander/output/trimui/DinguxCommander" "$(BUILD_PATH)/SDCARD/Tools/Commander.pak"
	cp -R "third-party/DinguxCommander/res" "$(BUILD_PATH)/SDCARD/Tools/Commander.pak"
	
poweroff:
	mkdir -p "$(BUILD_PATH)/SDCARD/Tools"
	cp -R "paks/Power Off.pak" "$(BUILD_PATH)/SDCARD/Tools"

# CORE

emus: gb pm ngp gg

gb:
	mkdir -p "$(BUILD_PATH)/SDCARD/Emus"
	mkdir -p "$(BUILD_PATH)/SDCARD/Roms/Game Boy"
	cd ./third-party/gambatte-dms && make
	cp -R "paks/Game Boy.pak" "$(BUILD_PATH)/SDCARD/Emus"
	cp "third-party/gambatte-dms/gambatte-dms" "$(BUILD_PATH)/SDCARD/Emus/Game Boy.pak"

pm:
	mkdir -p "$(BUILD_PATH)/SDCARD/Emus"
	mkdir -p "$(BUILD_PATH)/SDCARD/Roms/Pokemon Mini"
	cd ./third-party/pokemini/platform/trimui && make
	cp -R "paks/Pokemon Mini.pak" "$(BUILD_PATH)/SDCARD/Emus"
	cp "third-party/pokemini/platform/trimui/pokemini" "$(BUILD_PATH)/SDCARD/Emus/Pokemon Mini.pak"

ngp:
	mkdir -p "$(BUILD_PATH)/SDCARD/Emus"
	mkdir -p "$(BUILD_PATH)/SDCARD/Roms/Neo Geo Pocket"
	cd ./third-party/race && make
	cp -R "paks/Neo Geo Pocket.pak" "$(BUILD_PATH)/SDCARD/Emus"
	cp "third-party/race/race-od" "$(BUILD_PATH)/SDCARD/Emus/Neo Geo Pocket.pak"

gg:
	mkdir -p "$(BUILD_PATH)/SDCARD/Emus"
	mkdir -p "$(BUILD_PATH)/SDCARD/Roms/Game Gear"
	cd ./third-party/sms_sdl && make
	cp -R "paks/Game Gear.pak" "$(BUILD_PATH)/SDCARD/Emus"
	cp "third-party/sms_sdl/sms_sdl" "$(BUILD_PATH)/SDCARD/Emus/Game Gear.pak"

# EXTRAS

extras: gba sms gen stockui tyrian

gba:
	mkdir -p "$(BUILD_PATH)/EXTRAS/Emus"
	cd ./third-party/gpsp-bittboy/trimui && make
	cp -R "paks/Game Boy Advance.pak" "$(BUILD_PATH)/EXTRAS/Emus"
	cp "third-party/gpsp-bittboy/trimui/gpsp" "$(BUILD_PATH)/EXTRAS/Emus/Game Boy Advance.pak"
	cp "third-party/gpsp-bittboy/game_config.txt" "$(BUILD_PATH)/EXTRAS/Emus/Game Boy Advance.pak"

sms:
	mkdir -p "$(BUILD_PATH)/EXTRAS/Emus"
	cd ./third-party/sms_sdl && make
	cp -R "paks/Master System.pak" "$(BUILD_PATH)/EXTRAS/Emus"
	cp "third-party/sms_sdl/sms_sdl" "$(BUILD_PATH)/EXTRAS/Emus/Master System.pak"

gen:
	mkdir -p "$(BUILD_PATH)/EXTRAS/Emus"
	cp -R "paks/Genesis.pak" "$(BUILD_PATH)/EXTRAS/Emus"

stockui:
	mkdir -p "$(BUILD_PATH)/EXTRAS/Tools"
	cp -R "paks/StockUI.pak" "$(BUILD_PATH)/EXTRAS/Tools"

tyrian:
	mkdir -p "$(BUILD_PATH)/EXTRAS/Games"
	cp -R "paks/OpenTyrian.pak" "$(BUILD_PATH)/EXTRAS/Games"

# LIBMMENU

lib:
	cd ./src/libmmenu && make

# TRIMUI

trimui: lib
	mkdir -p "$(BUILD_PATH)"
	cp "src/libmmenu/libmmenu.so" "TrimuiUpdate/install"
	cd ./TrimuiUpdate/install && make
	rm -f "TrimuiUpdate/install/libmmenu.so"
	mv "TrimuiUpdate/install/TrimuiUpdate_installMinUI.zip" "$(BUILD_PATH)"
	cd ./TrimuiUpdate/uninstall && make
	mv "TrimuiUpdate/uninstall/TrimuiUpdate_uninstallMinUI.zip" "$(BUILD_PATH)"

clean:
	rm -rf ./build
	cd ./src/ui && make clean
	cd ./src/encode && make clean
	cd ./src/show && make clean
	cd ./third-party/DinguxCommander && make clean
	cd ./third-party/gambatte-dms && make clean
	cd ./third-party/pokemini/platform/trimui && make clean
	cd ./third-party/race && make clean
	cd ./third-party/sms_sdl && make clean
	cd ./third-party/gpsp-bittboy/trimui && make clean
