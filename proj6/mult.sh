# #DGX  
# for t in 8 32 128 256 512
# 	do
# 	for n in 2 8 16
# 	do 
# 	g++ -DNMB=$n -DLOCAL_SIZE=$t -o proj first.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
# ./proj
# done
# done

#!/bin/bash
# number of threads:
# for t in 8 32 64 128 256 512
# do
# 	echo LOCAL_SIZE = $t
# 	# number of subdivisions:
# 	for s in  1024 32768 131072 262144 524288 1048576 4194304 8388608
# 	do
# 		echo GLOBAL_SIZE = $s
# 		g++ -DLOCAL_SIZE=$t -DGLOBAL_SIZE=$s multiply.cpp -o prog /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
# 		./prog
# 	done
# done

#!/bin/bash

# number of threads
for gs in 1024 32768 131072 262144 524288 1048576 4194304 8388608
do
    # number of subdivisions:
    for ls in 32 64 128 256
    do
	let ng=$gs/$ls
        g++ -DNUM_ELEMENTS=$gs -DLOCAL_SIZE=$ls -DNUM_WORK_GROUPS=$ng -o mult mult.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
        ./mult
    done
done
rm mult