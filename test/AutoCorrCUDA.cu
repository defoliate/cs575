// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// CUDA runtime
#include <cuda_runtime.h>
// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"

#ifndef BLOCKSIZE
#define BLOCKSIZE		128     // number of threads per block
#endif

#define NUMBLOCKS		256

__global__  void AutoCorr( float *dA, float *dSums )
{
	int gid   = blockIdx.x*blockDim.x + threadIdx.x;
	int shift = gid;

	float sum = 0.;
	for( int i = 0; i < 32768; i++ )
	{
		sum += dA[i] * dA[i + shift];
	}
	dSums[shift] = sum;
}

int
main( int argc, char *argv[ ] )
{

// Reading Data from file

FILE *fp = fopen( "signal.txt", "r" );
if( fp == NULL )
{
	fprintf( stderr, "Cannot open file 'signal.txt'\n" );
	exit( 1 );
}

int Size;
fscanf( fp, "%d", &Size );
float *hA =     new float[ 2*Size ];
float *hSums  = new float[ 1*Size ];
for( int i = 0; i < Size; i++ )
{
	fscanf( fp, "%f", &hA[i] );
	hA[i+Size] = hA[i];		// duplicate the array
}
fclose( fp );

// Allocate Device memory
cudaError_t status;
float *dA, *dSums;

status = cudaMalloc( (void **)(&dA), 2*Size*sizeof(float) );
checkCudaErrors( status );

status = cudaMalloc( (void **)(&dSums), Size*sizeof(float) );
checkCudaErrors( status );

// copy host memory to device
status = cudaMemcpy( dA, hA, 2*Size*sizeof(float), cudaMemcpyHostToDevice );
checkCudaErrors( status );

status = cudaMemcpy( dSums, hSums, Size*sizeof(float), cudaMemcpyHostToDevice );
checkCudaErrors( status );

// setup the execution parameters:

dim3 threads(BLOCKSIZE, 1, 1 );
dim3 grid(   NUMBLOCKS, 1, 1 );

	// create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	status = cudaEventCreate( &start );
	checkCudaErrors( status );
	status = cudaEventCreate( &stop );
	checkCudaErrors( status );

	// record the start event:

	status = cudaEventRecord( start, NULL );
	checkCudaErrors( status );

	// execute the kernel:

	AutoCorr<<< grid,threads >>>(dA,dSums);
// record the stop event:

status = cudaEventRecord( stop, NULL );
checkCudaErrors( status );

// wait for the stop event to complete:

status = cudaEventSynchronize( stop );
checkCudaErrors( status );

float msecTotal = 0.0f;
status = cudaEventElapsedTime( &msecTotal, start, stop );
checkCudaErrors( status );
double secondsTotal = 0.001 * (double)msecTotal;
//compute performance

double Performance = 2*(double)Size*(double)Size/ ( secondsTotal ) / 1000000.;
fprintf( stderr,"Performance = %8.2lf MegaOperations/Sec\n", Performance );

//Saving the Sums array in a matlab m file
// std::ofstream FileTemp;
// FileTemp.open("FSums.m");
// FileTemp << "Sums = [";
// FileTemp << Sums[0];
// for( int k = 1; k < Size; k++)
// {
// 	FileTemp << "," << Sums[k];
// }
// FileTemp<<"];";
// FileTemp.close();
}