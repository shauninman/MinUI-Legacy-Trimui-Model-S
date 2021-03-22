#!/bin/sh
# PlayStation.pak/post.sh

DIR=$(dirname "$0")
cd "$DIR"

# create bios folder and copy default bios from nand
if [ !-f ./bios/scph5502.bin ]; then
	mkdir -p ./bios
	cp "/usr/trimui/bin/pcsx_rearmed/bios/scph5502.bin" ./bios
fi