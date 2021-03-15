#!/bin/sh
# Super Nintendo.pak/launch.sh

EMU_EXE=snes9x2002
EMU_DIR=$(dirname "$0")
ROM_DIR=${EMU_DIR/.pak/}
ROM_DIR=${ROM_DIR/Emus/Roms}
EMU_NAME=${ROM_DIR/\/mnt\/SDCARD\/Roms\//}
ROM=${1}

HOME="$ROM_DIR"
cd "$HOME"
"$EMU_DIR/$EMU_EXE" "$ROM" &> "/mnt/SDCARD/.logs/$EMU_NAME.txt"
