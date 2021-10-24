#!/bin/sh
# Repair Filesystem.pak/launch.sh

DIR=$(dirname "$0")
cd "$DIR"

a=`ls /usr/sbin/ | grep fsck.vfat`
if [ "$a" == "" ]; then
  cp -f fsck.vfat /usr/sbin/fsck.vfat
  chmod 777 /usr/sbin/fsck.vfat
fi

show "$DIR/scanning.png"

runit=$(/usr/sbin/fsck.vfat -a -w /dev/mmcblk0p1)
b=`echo $runit | grep Dirty`

if [ "$b" == "" ]; then
  confirm "$DIR/doneclean.png"
else
  confirm "$DIR/donedirty.png"
fi

if [ -f ./OKAY ]; then
 rm -f ./OKAY
fi