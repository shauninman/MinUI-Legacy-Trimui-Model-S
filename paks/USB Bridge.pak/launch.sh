#!/bin/sh
# USB Bridge.pak/launch.sh

DIR=$(dirname "$0")
cd "$DIR"

a=`ps | grep adbd | grep -v grep`
if [ "$a" == "" ]; then
	confirm "$DIR/start.png"
	if [ -f ./OKAY ]; then
		show "$DIR/starting.png"
		/etc/init.d/adbd start &
		touch /tmp/disable-sleep
	fi
else
	confirm "$DIR/stop.png"
	if [ -f ./OKAY ]; then
		show "$DIR/stopping.png"
		killall adbd
		rm -f /tmp/disable-sleep
	fi
fi