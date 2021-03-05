# TODO: this should be a build script, not a makefile, especially as the number of paks grow

.PHONY: build
.PHONY: clean

build: 
	cd ./src/ui && make
	cd ./src/encode && make
	cd ./src/show && make
	cd ./src/libmmenu && make
	
	cp "src/libmmenu/libmmenu.so" "TrimuiUpdate/install"
	cd ./TrimuiUpdate/install && make
	cd ./TrimuiUpdate/uninstall && make
	
	cd ./third-party/DinguxCommander && make
	
	cd ./third-party/gambatte-dms && make
	cd ./third-party/pokemini/platform/trimui && make
	cd ./third-party/race && make
	cd ./third-party/sms_sdl && make
	cd ./third-party/gpsp-bittboy/trimui && make
	
	cp "src/ui/MinUI" "SDCARD/System.pak"
	cp "src/encode/encode" "SDCARD/System.pak"
	cp "src/show/show" "SDCARD/System.pak"
	
	cp "TrimuiUpdate/install/TrimuiUpdate_installMinUI.zip" "SDCARD"
	cp "TrimuiUpdate/uninstall/TrimuiUpdate_uninstallMinUI.zip" "SDCARD"
	
	cp "third-party/DinguxCommander/output/trimui/DinguxCommander" "SDCARD/Tools/Commander.pak"
	cp -R "third-party/DinguxCommander/res" "SDCARD/Tools/Commander.pak"
	
	cp "third-party/gambatte-dms/gambatte-dms" "SDCARD/Emus/Game Boy.pak"
	cp "third-party/pokemini/platform/trimui/pokemini" "SDCARD/Emus/Pokemon Mini.pak"
	cp "third-party/race/race-od" "SDCARD/Emus/Neo Geo Pocket.pak"
	cp "third-party/sms_sdl/sms_sdl" "SDCARD/Emus/Game Gear.pak"
	cp "third-party/sms_sdl/sms_sdl" "SDCARD/Emus/Master System.pak"
	cp "third-party/gpsp-bittboy/trimui/gpsp" "SDCARD/Emus/Game Boy Advance.pak"
	cp "third-party/gpsp-bittboy/game_config.txt" "SDCARD/Emus/Game Boy Advance.pak"

clean:
	cd ./src/ui && make clean
	cd ./src/encode && make clean
	cd ./src/show && make clean
	cd ./src/libmmenu && make clean
	
	cd ./TrimuiUpdate/install && make clean
	cd ./TrimuiUpdate/uninstall && make clean
	
	cd ./third-party/DinguxCommander && make clean
	
	cd ./third-party/gambatte-dms && make clean
	cd ./third-party/pokemini/platform/trimui && make clean
	cd ./third-party/race && make clean
	cd ./third-party/sms_sdl && make clean
	cd ./third-party/gpsp-bittboy/trimui && make clean