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
/usr/sbin/fsck.vfat -a -w /dev/mmcblk0p1
confirm "$DIR/done.png"
if [ -f ./OKAY ]; then
 rm -f ./OKAY
fi
