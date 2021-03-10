#!/bin/sh

cd /mnt/SDCARD/System.pak

mkdir -p /mnt/SDCARD/.logs

./MinUI &> "/mnt/SDCARD/.logs/MinUI.txt"
sync

NEXT=./next.sh
if [ -f $NEXT ] ; then
	cmd=`cat $NEXT`
	rm -f $NEXT
	killall keymon
	eval $cmd
	sync
	keymon &
fi
