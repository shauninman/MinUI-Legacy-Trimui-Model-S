#!/bin/sh
# System.pak/launch.sh

a=`ps | grep keymon | grep -v grep`
if [ "$a" == "" ]; then
	keymon &
fi

SD=/mnt/SDCARD

UPDATE_LOG="$SD/update.log"
UPDATE_ZIP="$SD/TrimuiUpdate_MinUI.zip"
UPDATE_TMP="$SD/.tmp_update"

mkdir -p "$SD/.logs"
cd "$SD/System.pak"

# TODO: just search .tmp_update for any .pak
if [ -d "$UPDATE_TMP/Emus" ] || [ -d "$UPDATE_TMP/Games" ] || [ -d "$UPDATE_TMP/Tools" ]; then
	./update.sh
fi

notify 100 quit
killall -s KILL updateui
rm -f "$SD/update.log"

touch /tmp/minui_exec
sync

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/mnt/SDCARD/System.pak/lib"

while [ -f /tmp/minui_exec ]; do
	./MinUI &> "$SD/.logs/MinUI.txt"
	sync

	NEXT=./next.sh
	if [ -f $NEXT ]; then
		CMD=`cat $NEXT`
		rm -f $NEXT
		eval $CMD
		sync
	fi
done

killall keymon
sync

if [ -f /tmp/minui_update ]; then
	rm -f /tmp/minui_update
	killall updater
	
	echo start updating | tee $UPDATE_LOG
	updateui >> $UPDATE_LOG &
	notify 0 "unzip Update"
	mkdir -p ${UPDATE_TMP}
	total=`unzip -l ${UPDATE_ZIP} | wc -l`
	unzip -d ${UPDATE_TMP} -o ${UPDATE_ZIP} | awk -v total="$total" -v out="/tmp/.update_msg" 'function bname(file,a,n){n=split(file,a,"/");return a[n]}BEGIN{cnt=0}{printf "">out;cnt+=1;printf "%d unzip %s\n",cnt*100/total,bname($2)>>out;close(out)}'
	
	updateui >> $UPDATE_LOG &
	"$UPDATE_TMP/updater" | tee -a $UPDATE_LOG
fi