#!/bin/bash
numt=(1 2 3 4)
arraysize=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
file="maxSIMDR.csv"

echo "Threads, Size, SIMD, Standard, SIMD+T, Standard+T" > $file

for t in ${numt[@]}
do
    for a in ${arraysize[@]}
    do
        echo -n "${t}, " >> $file
        g++ -DARRAYSIZE=$a -o proj simd.cpp -lm -fopenmp
        ./proj >> $file
    done
done
rm "proj"
