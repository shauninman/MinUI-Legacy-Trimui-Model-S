#!/bin/sh
# Sega 32X.pak/launch.sh

REMAP_FROM="Sega 32X"
REMAP_ONTO="Genesis"
EMU_DIR=$(dirname "$0")
EMU_DIR=${EMU_DIR/$REMAP_FROM/$REMAP_ONTO}
ROM=${1}

"$EMU_DIR/launch.sh" "$ROM"