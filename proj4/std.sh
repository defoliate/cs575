#!/bin/bash
arraysize=(2000 4000 8000 16000 31250 62500 125000 250000 500000 1000000 2000000 4000000 8000000)
file="maxSIMD.csv"

echo "Size, SIMD, Standard" > $file

for a in ${arraysize[@]}
do
    g++ -DARRAYSIZE=$a -o proj proj4.cpp -lm -fopenmp
    ./proj >> $file
done
rm "proj"
