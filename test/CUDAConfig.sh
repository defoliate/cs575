CUDA_NVCC=/usr/local/cuda/bin/nvcc 

$CUDA_NVCC AutoCorrCUDA.cu -o CUDAproj -lm 
./CUDAproj