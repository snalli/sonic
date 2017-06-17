#!/bin/bash

echo "** switching off tracing"
echo 0 > /sys/kernel/debug/tracing/tracing_on
echo nop > /sys/kernel/debug/tracing/current_tracer

# echo "** switching off all but one cpus"
# echo 0 > /sys/devices/system/cpu/cpu1/online 
# echo 0 > /sys/devices/system/cpu/cpu2/online 
# echo 0 > /sys/devices/system/cpu/cpu3/online 
# echo 0 > /sys/devices/system/cpu/cpu4/online 
# echo 0 > /sys/devices/system/cpu/cpu5/online 
# echo 0 > /sys/devices/system/cpu/cpu6/online 
# echo 0 > /sys/devices/system/cpu/cpu7/online 

# requires atleast 4GB of PM at /dev/pmem0
# mount at /mnt/dax
echo "** mounting dax"
umount /mnt/dax
mkfs.ext4 -F /dev/pmem0
mount -o dax /dev/pmem0 /mnt/dax

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
