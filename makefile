.PHONY: clean

all: base extra

base: sd system tools emus trimui
	
extra: extras gba sms

# DIRS

extras: 
	mkdir -p "dist/EXTRAS"

sd:
	mkdir -p "dist/SDCARD/Emus"
	mkdir -p "dist/SDCARD/Logs"
	mkdir -p "dist/SDCARD/Roms"
	mkdir -p "dist/SDCARD/Tools"

# MINUI

system:
	cd ./src/ui && make
	cd ./src/encode && make
	cd ./src/show && make
	cp -R "paks/System.pak" "dist/SDCARD"
	cp "src/ui/MinUI" 		"dist/SDCARD/System.pak"
	cp "src/encode/encode" 	"dist/SDCARD/System.pak"
	cp "src/show/show" 		"dist/SDCARD/System.pak"

readme:
	cp "readme.md" "dist"

# TOOLS

tools: commander poweroff

commander:
	cd ./third-party/DinguxCommander && make
	cp -R "paks/Commander.pak" "dist/SDCARD/Tools"
	cp "third-party/DinguxCommander/output/trimui/DinguxCommander" "dist/SDCARD/Tools/Commander.pak"
	cp -R "third-party/DinguxCommander/res" "dist/SDCARD/Tools/Commander.pak"
	
poweroff:
	cp -R "paks/Power Off.pak" "dist/SDCARD/Tools"

# CORE

emus: gb pm ngp gg

gb:
	cd ./third-party/gambatte-dms && make
	cp -R "paks/Game Boy.pak" "dist/SDCARD/Emus"
	cp "third-party/gambatte-dms/gambatte-dms" "dist/SDCARD/Emus/Game Boy.pak"

pm:
	cd ./third-party/pokemini/platform/trimui && make
	cp -R "paks/Pokemon Mini.pak" "dist/SDCARD/Emus"
	cp "third-party/pokemini/platform/trimui/pokemini" "dist/SDCARD/Emus/Pokemon Mini.pak"

ngp:
	cd ./third-party/race && make
	cp -R "paks/Neo Geo Pocket.pak" "dist/SDCARD/Emus"
	cp "third-party/race/race-od" "dist/SDCARD/Emus/Neo Geo Pocket.pak"

gg:
	cd ./third-party/sms_sdl && make
	cp -R "paks/Game Gear.pak" "dist/SDCARD/Emus"
	cp "third-party/sms_sdl/sms_sdl" "dist/SDCARD/Emus/Game Gear.pak"

# EXTRAS

gba:
	cd ./third-party/gpsp-bittboy/trimui && make
	cp -R "paks/Game Boy Advance.pak" "dist/EXTRAS"
	cp "third-party/gpsp-bittboy/trimui/gpsp" "dist/EXTRAS/Game Boy Advance.pak"
	cp "third-party/gpsp-bittboy/game_config.txt" "dist/EXTRAS/Game Boy Advance.pak"

sms:
	cd ./third-party/sms_sdl && make
	cp -R "paks/Master System.pak" "dist/EXTRAS"
	cp "third-party/sms_sdl/sms_sdl" "dist/EXTRAS/Master System.pak"

# LIBMMENU

lib:
	cd ./src/libmmenu && make

# TRIMUI

trimui: lib
	cp "src/libmmenu/libmmenu.so" "TrimuiUpdate/install"
	cd ./TrimuiUpdate/install && make
	rm -f "TrimuiUpdate/install/libmmenu.so"
	mv "TrimuiUpdate/install/TrimuiUpdate_installMinUI.zip" "dist"
	cd ./TrimuiUpdate/uninstall && make
	mv "TrimuiUpdate/uninstall/TrimuiUpdate_uninstallMinUI.zip" "dist"

clean:
	rm -rf ./dist
	cd ./src/ui && make clean
	cd ./src/encode && make clean
	cd ./src/show && make clean
	cd ./third-party/DinguxCommander && make clean
	cd ./third-party/gambatte-dms && make clean
	cd ./third-party/pokemini/platform/trimui && make clean
	cd ./third-party/race && make clean
	cd ./third-party/sms_sdl && make clean
	cd ./third-party/gpsp-bittboy/trimui && make clean
