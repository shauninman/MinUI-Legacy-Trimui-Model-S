#!/bin/sh
# Genesis.pak/launch.sh

EMU_EXE=PicoDrive
EMU_DIR=$(dirname "$0")
ROM_DIR=${EMU_DIR/.pak/}
ROM_DIR=${ROM_DIR/Emus/Roms}
EMU_NAME=${ROM_DIR/\/mnt\/SDCARD\/Roms\//}
ROM=${1}

encode "$ROM"

HOME="$ROM_DIR"
cd "$HOME"
"/usr/trimui/bin/$EMU_EXE" "$ROM"  &> "/mnt/SDCARD/.logs/$EMU_NAME.txt"
