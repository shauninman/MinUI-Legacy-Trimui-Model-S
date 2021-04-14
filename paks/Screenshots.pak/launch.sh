#!/bin/sh
# Flipbook.pak/launch.sh

DIR=$(dirname "$0")
cd "$DIR"

flipbook "/mnt/SDCARD/.minui/screenshots" &> "/mnt/SDCARD/.minui/logs/Flipbook.txt"