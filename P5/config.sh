CUDA_PATH=/usr/local/apps/cuda/cuda-10.1
CUDA_BIN_PATH=$CUDA_PATH/bin
CUDA_NVCC=$CUDA_BIN_PATH/nvcc
for b in 16 32 64 128
do


	for n in 16384 32768 65536 131072 262144 524288 1048576
	do

		$CUDA_NVCC -DBLOCKSIZE=$b -DNUMTRIALS=$n main.cu -o proj
		./proj
	done
done
