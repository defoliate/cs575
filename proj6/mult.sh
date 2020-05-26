for t in 1024 32768 131072 262144 524288 1048576 4194304 8388608
	do
	for n in 32 64 128 256
	do 
	g++ -DNMB=$n -DLOCAL_SIZE=$t -o proj mult.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
./proj
done
done