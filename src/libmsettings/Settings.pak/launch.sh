#!/bin/sh
# Share.pak/launch.sh for testing

DIR=$(dirname "$0")
cd "$DIR"

./msettings_host &> "/mnt/SDCARD/.minui/logs/msettings_host.txt" &
./msettings_client &> "/mnt/SDCARD/.minui/logs/msettings_client.txt"
killall msettings_host