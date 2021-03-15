#!/bin/sh
# Neo Geo Pocket Color.pak/launch.sh

REMAP_FROM="Neo Geo Pocket Color"
REMAP_ONTO="Neo Geo Pocket"
EMU_DIR=$(dirname "$0")
EMU_DIR=${EMU_DIR/$REMAP_FROM/$REMAP_ONTO}
ROM=${1}

"$EMU_DIR/launch.sh" "$ROM"