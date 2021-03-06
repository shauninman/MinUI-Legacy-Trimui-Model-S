#!/bin/sh

EMU_EXE=snes9x4d
EMU_DIR=$(dirname "$0")
ROM_DIR=${EMU_DIR/.pak/}
ROM_DIR=${ROM_DIR/Emus/Roms}
EMU_NAME=${ROM_DIR/\/mnt\/SDCARD\/Roms\//}
ROM=${1}

HOME="$ROM_DIR"
cd "$HOME"
"$EMU_DIR/$EMU_EXE" "$ROM" &> "/mnt/SDCARD/Logs/$EMU_NAME.txt"
