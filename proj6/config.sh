#DGX  
for t in 8 32 128 256 512
	do
	for n in 1 2 3 4 5 6 7
	do 
	g++ -DNMB=$n -DLOCAL_SIZE=$t -o proj first.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -lm -fopenmp
./proj
done
done
