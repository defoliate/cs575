#!/bin/bash
# number of threads:
for t in 64 128 256
do
	echo LOCAL_SIZE = $t
	# number of subdivisions:
	for s in  1024 32768 131072 262144 524288 1048576 2097152 4194304 8388608
	do
		echo GLOBAL_SIZE = $s
		g++ -DLOCAL_SIZE=$t -DGLOBAL_SIZE=$s multaddreduction.cpp -o prog3 /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
		./prog3 >> a.txt
	done
    echo -e >> a.txt
done