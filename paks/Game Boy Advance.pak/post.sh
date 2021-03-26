#!/bin/sh
# Game Boy Advance.pak/post.sh

DIR=$(dirname "$0")
cd "$DIR"

# copy default bios from nand
if [ ! -f ./gba_bios.bin ]; then
	cp "/usr/trimui/bin/gba_bios.bin" ./
fi