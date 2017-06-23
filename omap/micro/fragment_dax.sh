#!/bin/bash

#echo "** fragmenting dax"
./chopper/src/src/puncher /mnt/dax/puncher  ./chopper/src/src/puncher.conf 0

# echo "** creating large file"
# largefile=/mnt/dax/largefile
# block=4096
# count=6291456
# dd if=/dev/zero of=$largefile bs=$block count=$count
# printf "**"; filefrag $largefile

echo "** drop caches"
sync;sync;sync
echo 3 > /proc/sys/vm/drop_caches
echo 3 > /proc/sys/vm/drop_caches
echo 3 > /proc/sys/vm/drop_caches
echo 3 > /proc/sys/vm/drop_caches
