#!/bin/sh
# Wipe Stock.pak/launch.sh

DIR=$(dirname "$0")
cd "$DIR"

confirm "$DIR/delete.png"

if [ -f ./OKAY ]; then
	SD=/mnt/SDCARD
	swapoff -a
	rm -f "$SD/cachefile"
	rm -f "$SD/Roms/recentlist.json"
	rm -f "$SD/update.log"

	rm -rf "$SD/Apps"
	rm -rf "$SD/Imgs"
	rm -rf "$SD/Roms/ARCADE"
	rm -rf "$SD/Roms/FC"
	rm -rf "$SD/Roms/GB"
	rm -rf "$SD/Roms/GBA"
	rm -rf "$SD/Roms/MD"
	rm -rf "$SD/Roms/MS"
	rm -rf "$SD/Roms/NEOGEO"
	rm -rf "$SD/Roms/PCE"
	rm -rf "$SD/Roms/PS"
	rm -rf "$SD/Roms/SFC"
fi