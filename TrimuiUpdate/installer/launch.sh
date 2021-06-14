#!/bin/sh
# .tmp_update/updater (persistent)

if [ -f /mnt/SDCARD/System/System.pak/launch.sh ]; then
	/mnt/SDCARD/System/System.pak/launch.sh
else
	notify 50 "unable to find MinUI"
	sleep 2
	notify 100 "returning to stock"
	sleep 2
	notify 100 quit
	killall -s KILL updateui
fi