#!/bin/sh

sync

DIR=$(dirname "$0")
cd "$DIR"
/mnt/SDCARD/System.pak/show "$DIR/poweroff.png"

poweroff -d 1