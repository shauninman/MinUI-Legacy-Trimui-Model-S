#!/bin/sh

SD=/mnt/SDCARD
SWAP=$SD/.minui/swapfile
if [ ! -f "$SWAP" ]; then
	show $SD/System/res/swap.png
	dd if=/dev/zero of=$SWAP bs=1M count=128
	chmod 600 $SWAP
	mkswap $SWAP
fi
swapon $SWAP

touch /tmp/using-swap