#!/bin/bash

# number of threads
for t in 1 2 4 8
do
    echo NUMT = $t
    for a in 100 10000 100000 1000000
    do
        g++ -DNUMT=$t -DNUMTRIALS=$a  p1.cpp -o p1.exe -lm -fopenmp
        ./p1.exe
    done
done

rm p1.exe
