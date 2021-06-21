#!/bin/sh
# System.pak/launch.sh

# echo -e "LD_LIBRARY_PATH=$LD_LIBRARY_PATH\nPATH=$PATH" > /mnt/SDCARD/.minui/logs/System.txt

killall keymon

export LD_LIBRARY_PATH="/mnt/SDCARD/System/lib:$LD_LIBRARY_PATH"
export PATH="/mnt/SDCARD/System/bin:$PATH"

a=`ps | grep keymon | grep -v grep`
if [ "$a" == "" ]; then
	keymon &
fi

SD=/mnt/SDCARD

UPDATE_LOG="$SD/update.log"
UPDATE_ZIP="$SD/TrimuiUpdate_MinUI.zip"
UPDATE_TMP="$SD/.tmp_update"

cd "$SD/System/System.pak"

# TODO: just search .tmp_update for any .pak
if [ -d "$UPDATE_TMP/Emus" ] || [ -d "$UPDATE_TMP/Games" ] || [ -d "$UPDATE_TMP/Tools" ]; then
	./update.sh
fi

notify 100 quit
killall -s KILL updateui
# killall -s KILL tee
rm -f "$UPDATE_LOG"

touch /tmp/minui_exec
sync

while [ -f /tmp/minui_exec ]; do
	# these can be deleted with Commander.pak so make sure they exist
	mkdir -p "$SD/.minui/logs"
	mkdir -p "$SD/.minui/screenshots"
	
	./MinUI &> "$SD/.minui/logs/MinUI.txt"
	sync

	NEXT="$SD/.minui/next.sh"
	if [ -f $NEXT ]; then
		CMD=`cat $NEXT`
		rm -f $NEXT
		eval $CMD
		
		if [ -f /tmp/using-swap ]; then
			rm -f /tmp/using-swap
			swapoff -a
		fi
		sync
	fi
done

killall keymon

if [ -f /tmp/minui_update ]; then
	rm -f /tmp/minui_update
	killall -s KILL updater
	
	echo start updating | tee $UPDATE_LOG
	updateui >> $UPDATE_LOG &
	notify 0 "extracting package"
	mkdir -p ${UPDATE_TMP}
	total=`unzip -l ${UPDATE_ZIP} | wc -l`
	unzip -d ${UPDATE_TMP} -o ${UPDATE_ZIP} | awk -v total="$total" -v out="/tmp/.update_msg" 'function bname(file,a,n){n=split(file,a,"/");return a[n]}BEGIN{cnt=0}{printf "">out;cnt+=1;printf "%d extract %s\n",cnt*100/total,bname($2)>>out;close(out)}'
	"$UPDATE_TMP/updater" | tee -a $UPDATE_LOG
fi