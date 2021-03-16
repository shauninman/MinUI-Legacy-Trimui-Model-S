#!/bin/sh
# Power Off.pak/launch.sh

sync

DIR=$(dirname "$0")
cd "$DIR"
/mnt/SDCARD/System.pak/bin/show "$DIR/poweroff.png"

poweroff -d 1