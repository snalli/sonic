#!/bin/bash

time=""

$time ./dax-access -b4
$time ./dax-access -b32
$time ./dax-access -b64

./dax-access -m -b4
./dax-access -m -b32
./dax-access -m -b64

$time ./dax-access -w -b4
$time ./dax-access -w -b32
$time ./dax-access -w -b64

./dax-access -wm -b4
./dax-access -wm -b32
./dax-access -wm -b64
