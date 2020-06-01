#!/bin/bash

# number of threads
for gs in 1024 32768 131072 262144 524288 1048576 4194304 8388608
do
    # number of subdivisions:
    for ls in 32 64 128 256
    do
	let ng=$gs/$ls
        g++ -DNUM_ELEMENTS=$gs -DLOCAL_SIZE=$ls -DNUM_WORK_GROUPS=$ng -o multReduce multReduce.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
        ./multReduce
    done
done

rm multReduce
