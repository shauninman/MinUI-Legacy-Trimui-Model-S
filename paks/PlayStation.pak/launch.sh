#!/bin/sh
# PlayStation.pak/launch.sh

EMU_EXE=pcsx
EMU_DIR=$(dirname "$0")
ROM_DIR=${EMU_DIR/.pak/}
ROM_DIR=${ROM_DIR/Emus/Roms}
EMU_NAME=${ROM_DIR/\/mnt\/SDCARD\/Roms\//}
ROM=${1}

HOME="$ROM_DIR"
cd "$EMU_DIR"
"$EMU_DIR/$EMU_EXE" -cdfile "$ROM" &> "/mnt/SDCARD/.logs/$EMU_NAME.txt"