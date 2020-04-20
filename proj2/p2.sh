#!/bin/bash

numthreads=(1 2 4 8 16)
numnodes=(2 4 8 16 32 64 128 256 512)
numtries=100
file="maxMegaMults.txt"

echo -n "Trials, " > $file

for t in ${numnodes[@]}
do
     echo -n "${t}, area, " >> $file
done
echo -e "\n" >> $file


for t in ${numthreads[@]}
do
     echo -n "${t}, " >> $file
     for s in ${numnodes[@]}
     do
          g++ -DNUMT=$t -DNUMNODES=$s -DNUMTRIES=$numtries -o proj main.cpp -O3 -lm -fopenmp
          ./proj >> $file
          #./proj
     done
     echo -e "\n" >> $file
done