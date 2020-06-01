#!/bin/bash
arraysize=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
file="maxSIMD.csv"

echo "Size, SIMD, Standard" > $file

for a in ${arraysize[@]}
do
    g++ -DARRAYSIZE=$a -o proj proj4.cpp -lm -fopenmp
    ./proj >> $file
done
rm "proj"
