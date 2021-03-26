#!/bin/sh
# TurboGrafx-16.pak/launch.sh

EMU_EXE=temper
EMU_DIR=$(dirname "$0")
ROM_DIR=${EMU_DIR/.pak/}
ROM_DIR=${ROM_DIR/Emus/Roms}
EMU_NAME=${ROM_DIR/\/mnt\/SDCARD\/Roms\//}
ROM=${1}

HOME="$ROM_DIR"
cd "$EMU_DIR"
"$EMU_DIR/$EMU_EXE" "$ROM" &> "/mnt/SDCARD/.minui/logs/$EMU_NAME.txt"