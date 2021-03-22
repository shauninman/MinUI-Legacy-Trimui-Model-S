#!/bin/sh
# USB Bridge.pak/launch.sh

DIR=$(dirname "$0")

show "$DIR/starting.png"

/etc/init.d/adbd start &