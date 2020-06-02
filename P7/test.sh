#!/bin/bash

#openmp,1&32 thread
g++ openmp.cpp -o prog -lm -fopenmp
./prog>>p7openmp.csv
rm prog

#simd
g++ simd.cpp simd.p4.cpp -o prog -lm -fopenmp
./prog >> p7simd.csv
rm prog

#opencl,need GPU,should run in rabbit.
g++ opencl.cpp  -o prog /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
./prog>>p7opencl.csv
rm prog
