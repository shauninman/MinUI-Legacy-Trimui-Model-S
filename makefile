# minui/makefile

.PHONY: clean

BUILD_TIME!=date +%Y%m%d
RELEASE_PATH=./release
RELEASE_BASE=MinUI-$(BUILD_TIME)
RELEASE_DOT!=find release/. -name "$(RELEASE_BASE)*.zip" -printf '.' | wc -m
RELEASE_NAME=$(RELEASE_BASE)-$(RELEASE_DOT)
BUILD_PATH=./build
PAYLOAD_PATH=$(BUILD_PATH)/PAYLOAD
ROMS_PATH=$(BUILD_PATH)/Roms

#--------------------------------------
all: readme sys emus tools zip
#--------------------------------------

lib:
	cd ./src/libmmenu && make

readme:
	mkdir -p "$(BUILD_PATH)"
	echo "$(RELEASE_NAME)" > "$(BUILD_PATH)/version.txt"
	fmt -w 40 -s "src/MinUI/readme.txt" > "$(BUILD_PATH)/readme.txt"

sys: lib
	mkdir -p "$(PAYLOAD_PATH)"
	mkdir -p "$(PAYLOAD_PATH)/System"
	echo "$(RELEASE_NAME)" > "$(PAYLOAD_PATH)/System/version.txt"
	cd ./TrimuiUpdate/ && make
	cp -R ./TrimuiUpdate/installer/. "$(PAYLOAD_PATH)"
	cd ./src/MinUI && make
	cd ./src/show && make
	cd ./src/confirm && make
	cd ./src/flipbook && make
	cp -R "paks/System.pak" 		"$(PAYLOAD_PATH)/System"
	cp "src/MinUI/MinUI" 			"$(PAYLOAD_PATH)/System/System.pak"
	mkdir -p "$(PAYLOAD_PATH)/System/bin"
	mkdir -p "$(PAYLOAD_PATH)/System/lib"
	cp -R "res" "$(PAYLOAD_PATH)/System"
	cp "src/show/show" 				"$(PAYLOAD_PATH)/System/bin"
	cp "src/confirm/confirm" 		"$(PAYLOAD_PATH)/System/bin"
	cp "src/flipbook/flipbook" 		"$(PAYLOAD_PATH)/System/bin"
	cp "src/needs-swap.sh"			"$(PAYLOAD_PATH)/System/bin/needs-swap"
	cp "bin/keymon-patched"			"$(PAYLOAD_PATH)/System/bin/keymon"
	cp "src/libmmenu/libmmenu.so"	"$(PAYLOAD_PATH)/System/lib"
	cp -R "paks/Update.pak" 		"$(PAYLOAD_PATH)/System"

#--------------------------------------
emus: gb pm ngp gg snes ps gba nes gen pce swan lynx
#--------------------------------------

emu:
	mkdir -p "$(PAYLOAD_PATH)/Emus"

gb: emu
	mkdir -p "$(ROMS_PATH)/Game Boy"
	cd ./third-party/gambatte-dms && make -j
	cp -R "paks/Game Boy.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/gambatte-dms/gambatte-dms" "$(PAYLOAD_PATH)/Emus/Game Boy.pak"
	
	mkdir -p "$(ROMS_PATH)/Game Boy Color"
	cp -R "paks/Game Boy Color.pak" "$(PAYLOAD_PATH)/Emus"

pm: emu
	mkdir -p "$(ROMS_PATH)/Pokemon Mini"
	cd ./third-party/pokemini/platform/trimui && make -j
	cp -R "paks/Pokemon Mini.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/pokemini/platform/trimui/pokemini" "$(PAYLOAD_PATH)/Emus/Pokemon Mini.pak"

ngp: emu
	mkdir -p "$(ROMS_PATH)/Neo Geo Pocket"
	cd ./third-party/race && make -j
	cp -R "paks/Neo Geo Pocket.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/race/race-od" "$(PAYLOAD_PATH)/Emus/Neo Geo Pocket.pak"
	
	mkdir -p "$(ROMS_PATH)/Neo Geo Pocket Color"
	cp -R "paks/Neo Geo Pocket Color.pak" "$(PAYLOAD_PATH)/Emus"

gg: emu
	mkdir -p "$(ROMS_PATH)/Game Gear"
	cd ./third-party/sms_sdl && make -j
	cp -R "paks/Game Gear.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/sms_sdl/sms_sdl" "$(PAYLOAD_PATH)/Emus/Game Gear.pak"
	
	mkdir -p "$(ROMS_PATH)/Master System"
	cp -R "paks/Master System.pak" "$(PAYLOAD_PATH)/Emus"

snes: emu
	mkdir -p "$(ROMS_PATH)/Super Nintendo"
	cd ./third-party/snes9x2002 && make -j
	cp -R "paks/Super Nintendo.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/snes9x2002/snes9x2002" "$(PAYLOAD_PATH)/Emus/Super Nintendo.pak"

ps: emu
	mkdir -p "$(ROMS_PATH)/PlayStation"
	if [ ! -f ./third-party/pcsx_rearmed/config.mak ]; then cd ./third-party/pcsx_rearmed && CROSS_COMPILE=/opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi- ./configure --platform=trimui; fi
	cd ./third-party/pcsx_rearmed && make -j
	cp -R "paks/PlayStation.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/pcsx_rearmed/pcsx" "$(PAYLOAD_PATH)/Emus/PlayStation.pak"

gba: emu
	mkdir -p "$(ROMS_PATH)/Game Boy Advance"
	cd ./third-party/picogpsp && make -j
	cp -R "paks/Game Boy Advance.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/picogpsp/picogpsp" "$(PAYLOAD_PATH)/Emus/Game Boy Advance.pak"
	
nes: emu
	mkdir -p "$(ROMS_PATH)/Nintendo"
	cd ./third-party/fceux && make -j
	cp -R "paks/Nintendo.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/fceux/fceux/fceux.dge" "$(PAYLOAD_PATH)/Emus/Nintendo.pak"
	cp "third-party/fceux/fceux/backdrop.png" "$(PAYLOAD_PATH)/Emus/Nintendo.pak"
	cp "third-party/fceux/fceux/sp.bmp" "$(PAYLOAD_PATH)/Emus/Nintendo.pak"
	cp -R "third-party/fceux/fceux/palettes" "$(PAYLOAD_PATH)/Emus/Nintendo.pak"

gen: emu
	mkdir -p "$(ROMS_PATH)/Genesis"
	if [ ! -f ./third-party/picodrive/config.mak ]; then cd ./third-party/picodrive && CROSS_COMPILE=/opt/trimui-toolchain/bin/arm-buildroot-linux-gnueabi- ./configure --platform=trimui; fi
	cd ./third-party/picodrive && make -j
	cp -R "paks/Genesis.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/picodrive/PicoDrive" "$(PAYLOAD_PATH)/Emus/Genesis.pak"
	cp -R "third-party/picodrive/platform/trimui/skin" "$(PAYLOAD_PATH)/Emus/Genesis.pak"

pce: emu
	mkdir -p "$(ROMS_PATH)/TurboGrafx-16"
	cd ./third-party/temper/SDL && make -j
	cp -R "paks/TurboGrafx-16.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/temper/SDL/temper" "$(PAYLOAD_PATH)/Emus/TurboGrafx-16.pak"

swan: emu
	mkdir -p "$(ROMS_PATH)/WonderSwan"
	cd ./third-party/oswan && make -j
	cp -R "paks/WonderSwan.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/oswan/oswan" "$(PAYLOAD_PATH)/Emus/WonderSwan.pak"

lynx: emu
	mkdir -p "$(ROMS_PATH)/Lynx"
	cd ./third-party/handy-rs97 && make -j
	cp -R "paks/Lynx.pak" "$(PAYLOAD_PATH)/Emus"
	cp "third-party/handy-rs97/handy" "$(PAYLOAD_PATH)/Emus/Lynx.pak"

#--------------------------------------
tools: bridge commander poweroff reload stock # zero
#--------------------------------------

tool:
	mkdir -p "$(PAYLOAD_PATH)/Tools"

bridge: tool
	cp -R "paks/USB Bridge.pak" "$(PAYLOAD_PATH)/Tools"

commander: tool
	cd ./third-party/DinguxCommander && make -j
	cp -R "paks/Commander.pak" "$(PAYLOAD_PATH)/Tools"
	cp "third-party/DinguxCommander/output/trimui/DinguxCommander" "$(PAYLOAD_PATH)/Tools/Commander.pak"
	cp -R "third-party/DinguxCommander/res" "$(PAYLOAD_PATH)/Tools/Commander.pak"

poweroff: tool
	cp -R "paks/Power Off.pak" "$(PAYLOAD_PATH)/Tools"

stock: tool
	cp -R "paks/Stock UI.pak" "$(PAYLOAD_PATH)/Tools"

reload: tool
	cp -R "paks/Reload MinUI.pak" "$(PAYLOAD_PATH)/Tools"

zero: tool
	cp -R "paks/Zero Stock.pak" "$(PAYLOAD_PATH)/Tools"

#--------------------------------------

zip:
	mkdir -p "$(RELEASE_PATH)"
	cd "$(PAYLOAD_PATH)" && rm -f ../TrimuiUpdate_MinUI.zip
	cd "$(PAYLOAD_PATH)" && zip -r ../TrimuiUpdate_MinUI.zip . -x "*.DS_Store"
	cd "$(BUILD_PATH)" && zip -r ../release/$(RELEASE_NAME).zip readme.txt version.txt Roms TrimuiUpdate_MinUI.zip

#--------------------------------------

clean:
	cd ./src/libmmenu && make clean
	cd ./src/MinUI && make clean
	cd ./src/show && make clean
	cd ./src/confirm && make clean
	cd ./src/flipbook && make clean
	cd ./TrimuiUpdate/ && make clean
	cd ./third-party/DinguxCommander && make clean
	cd ./third-party/gambatte-dms && make clean
	cd ./third-party/pokemini/platform/trimui && make clean
	cd ./third-party/race && make clean
	cd ./third-party/sms_sdl && make clean
	cd ./third-party/snes9x2002 && make clean
	cd ./third-party/pcsx_rearmed && make clean
	cd ./third-party/gpsp-bittboy/trimui && make clean
	cd ./third-party/fceux && make clean
	cd ./third-party/picodrive && make clean
	cd ./third-party/temper/SDL && make clean
	cd ./third-party/oswan && make clean
	rm -rf ./build