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
	
	if [ -f "$DST/no-update" ]; then
		continue
	fi
	
	# TODO: add pre.sh?
	
	if [ -d "$DST" ]; then
		ACTION="update"
	else
		ACTION="install"
	fi

	notify $PERCENT "$ACTION $PAK_NAME"
	mkdir -p "$DST"
	cp -r "$SRC/." "$DST/" &
	wait $!
	sync
	
	POST="$DST/post.sh"
	if [ -f "$POST" ]; then
		notify $PERCENT "post-$ACTION"
		"$POST"
		rm -f "$POST"
	fi

	if [ "$ACTION" = "install" ]; then
		if [ "$SRC" != ${SRC/.\/Emus\//} ]; then
			ROM_DIR=${DST/.pak/}
			ROM_DIR=${ROM_DIR/Emus/Roms}
			if [ ! -d "$ROM_DIR" ]; then
				notify $PERCENT "add Roms/$PAK_NAME"
				mkdir -p "$ROM_DIR"
				sync
			fi
		fi
	fi
done

sync

notify 100 "paks done"
sleep 1

rm -rf ./Emus
rm -rf ./Tools

IFS=$SAVEIFS