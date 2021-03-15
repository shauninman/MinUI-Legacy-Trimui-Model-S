#!/bin/sh
# Master System.pak/launch.sh

REMAP_FROM="Master System"
REMAP_ONTO="Game Gear"
EMU_DIR=$(dirname "$0")
EMU_DIR=${EMU_DIR/$REMAP_FROM/$REMAP_ONTO}
ROM=${1}

"$EMU_DIR/launch.sh" "$ROM"