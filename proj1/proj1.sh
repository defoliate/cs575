#!/bin/bash

# number of threads
for t in 1 2 4 8
do
    echo NUMT = $t
    for a in 100 10000 100000 1000000
    do
        g++ -DNUMT=$t -DNUMTRIALS=$a  prog.cpp -o proj.exe -lm -fopenmp
        ./proj.exe
    done
done

rm proj.exe
