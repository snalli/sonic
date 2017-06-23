#!/bin/bash

user="snalli"
mnt="/mnt/dax"
pmem="/dev/pmem0"
cpu=1

echo "** switching off tracing"
echo 0 > /sys/kernel/debug/tracing/tracing_on
echo nop > /sys/kernel/debug/tracing/current_tracer

echo "** switching off all but one cpus"
echo $cpu > /sys/devices/system/cpu/cpu1/online 
echo $cpu > /sys/devices/system/cpu/cpu2/online 
echo $cpu > /sys/devices/system/cpu/cpu3/online 
echo $cpu > /sys/devices/system/cpu/cpu4/online 
echo $cpu > /sys/devices/system/cpu/cpu5/online 
echo $cpu > /sys/devices/system/cpu/cpu6/online 
echo $cpu > /sys/devices/system/cpu/cpu7/online 

# requires atleast 4GB of PM at /dev/pmem0
# mount at /mnt/dax
echo "** mounting dax"
umount $mnt
mkfs.ext4 -F $pmem
mount -o dax $pmem $mnt
chown -R $user $mnt
chgrp -R $user $mnt
