#!/bin/bash

g++ simd.cpp simd.p4.cpp -o prog /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
./prog >> p7simd.txt
rm prog

