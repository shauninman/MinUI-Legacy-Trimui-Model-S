#!/bin/sh
# System.pak/launch.sh

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/mnt/SDCARD/System/lib"
export PATH="/mnt/SDCARD/System/bin:$PATH"

a=`ps | grep keymon | grep -v grep`
if [ "$a" == "" ]; then
	keymon &
fi

SD=/mnt/SDCARD

UPDATE_LOG="$SD/update.log"
UPDATE_ZIP="$SD/TrimuiUpdate_MinUI.zip"
UPDATE_TMP="$SD/.tmp_update"

mkdir -p "$SD/.minui/logs"
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
	./MinUI &> "$SD/.minui/logs/MinUI.txt"
	sync

	NEXT="$SD/.minui/next.sh"
	if [ -f $NEXT ]; then
		CMD=`cat $NEXT`
		rm -f $NEXT
		eval $CMD
		sync
	fi
done

killall -s KILL keymon

if [ -f /tmp/minui_update ]; then
	rm -f /tmp/minui_update
	killall -s KILL updater
	
	echo start updating | tee $UPDATE_LOG
	updateui >> $UPDATE_LOG &
	notify 0 "unzip Update"
	mkdir -p ${UPDATE_TMP}
	total=`unzip -l ${UPDATE_ZIP} | wc -l`
	unzip -d ${UPDATE_TMP} -o ${UPDATE_ZIP} | awk -v total="$total" -v out="/tmp/.update_msg" 'function bname(file,a,n){n=split(file,a,"/");return a[n]}BEGIN{cnt=0}{printf "">out;cnt+=1;printf "%d unzip %s\n",cnt*100/total,bname($2)>>out;close(out)}'
	"$UPDATE_TMP/updater" | tee -a $UPDATE_LOG
fi