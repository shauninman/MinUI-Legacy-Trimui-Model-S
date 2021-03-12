#!/bin/sh

REMAP_FROM="Game Boy Color"
REMAP_ONTO="Game Boy"
EMU_DIR=$(dirname "$0")
EMU_DIR=${EMU_DIR/$REMAP_FROM/$REMAP_ONTO}
ROM=${1}

"$EMU_DIR/launch.sh" "$ROM"