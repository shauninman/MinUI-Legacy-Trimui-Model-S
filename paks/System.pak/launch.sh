#!/bin/sh

cd /mnt/SDCARD/System.pak

mkdir -p /mnt/SDCARD/Logs

./MinUI &> "/mnt/SDCARD/Logs/MinUI.txt"
sync

NEXT=./next.sh
if [ -f $NEXT ] ; then
	cmd=`cat $NEXT`
	rm -f $NEXT
	eval $cmd
	sync
fi
