#!/bin/sh
# System.pak/update.sh

SD="/mnt/SDCARD"
cd "$SD/.tmp_update"

mkdir -p "$SD/Emus"
mkdir -p "$SD/Roms"
mkdir -p "$SD/Tools"

NUM=0
TOTAL=`find . -name "*.pak" | wc -l`
PERCENT=0

SAVEIFS=$IFS
IFS=$(echo -en "\n\b")

notify $PERCENT "check paks"
sleep 1

for SRC in `find . -name "*.pak"` ; do
	DST="$SD/$SRC"
	DST=${DST/\/.\//\/} # replace `/./` with `/` in full path
	DST_DIR=$(dirname "$DST")
	PAK_NAME=$(basename "$SRC")
	PAK_NAME=${PAK_NAME/.pak/}
	
	NUM=`expr $NUM + 1`
	PERCENT=`expr $NUM \* 100 \/ $TOTAL`
	
	if [ -d "$DST" ]; then
		rm -rf "$DST"
		ACTION="update"
	else
		ACTION="install"
	fi

	notify $PERCENT "$ACTION $PAK_NAME"
	mv -f "$SRC" "$DST_DIR"

	if [ "$SRC" != ${SRC/.\/Emus\//} ]; then
		notify $PERCENT "add Roms/$PAK_NAME"
		ROM_DIR=${DST/.pak/}
		ROM_DIR=${ROM_DIR/Emus/Roms}
		mkdir -p "$ROM_DIR"
	fi
done

sync

notify 100 "paks done"
sleep 1


rm -rf ./Emus
rm -rf ./Tools

IFS=$SAVEIFS