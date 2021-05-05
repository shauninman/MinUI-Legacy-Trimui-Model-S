#!/bin/sh
# Game Boy Advance.pak/post.sh

DIR=$(dirname "$0")
cd "$DIR"

if [ -f gpsp ]; then
	# delete old emulator and assets
	rm -f gpsp
	rm -f gba_bios.bin
	rm -f game_config.txt
	# copy saves but leave originals
	cd "/mnt/SDCARD/Roms/Game Boy Advance"
	if [ -d .gpsp ]; then
		mkdir -p .picogpsp
		find . -name "*.sav" -exec cp {} .picogpsp/ \;
		rm -rf .mmenu
	fi
	cd -
fi