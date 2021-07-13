#!/bin/sh
# TurboGrafx-CD.pak/launch.sh

REMAP_FROM="TurboGrafx-CD"
REMAP_ONTO="TurboGrafx-16"
EMU_DIR=$(dirname "$0")
EMU_DIR=${EMU_DIR/$REMAP_FROM/$REMAP_ONTO}
ROM=${1}

"$EMU_DIR/launch.sh" "$ROM"