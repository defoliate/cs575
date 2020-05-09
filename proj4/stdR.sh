#!/bin/bash
numt=(2 4 8 16 32)
arraysize=(2000 4000 8000 16000 31250 62500 125000 250000 500000 1000000 2000000 4000000 8000000)
file="maxSIMDR.csv"

echo "Threads, Size, SIMD, Standard, SIMD+T, Standard+T" > $file

for t in ${numt[@]}
do
    for a in ${arraysize[@]}
    do
        echo -n "${t}, " >> $file
        g++ -DARRAYSIZE=$a -o proj simdt.cpp -lm -fopenmp
        ./proj >> $file
    done
done
rm "proj"