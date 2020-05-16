// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"


#ifndef BLOCKSIZE
#define BLOCKSIZE		32		// number of threads per block
#endif

#ifndef SIZE
#define SIZE			100000	// array size
#endif

#ifndef NUMTRIALS
#define NUMTRIALS		100	// to make the timing more accurate
#endif

#ifndef TOLERANCE
#define TOLERANCE		0.00001f	// tolerance to relative error
#endif


float chooseRand( float low, float high )
{
        float r = (float) rand();               // 0 - RAND_MAX
        float t = r  /  (float) RAND_MAX;       // 0. - 1.

        return   low  +  t * ( high - low );
}

void TimeOfDaySeed( )
{
	struct tm y2k = { 0 };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  timer;
	time( &timer );
	double seconds = difftime( timer, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );    // milliseconds
	srand( seed );
}


__global__  void ArrayMul( float *A, float *B, float *C, float *D )
{
	unsigned int gid = blockIdx.x*blockDim.x + threadIdx.x;

	
	// randomize the location and radius of the circle:
	float xc = A[gid];
	float yc = B[gid];
	float  r = C[gid];

	// solve for the intersection using the quadratic formula:
	float a = 2.;
	float b = -2.*( xc + yc );
	float c = xc*xc + yc*yc - r*r;
	float d = b*b - 4.*a*c;
	//If d is less than 0., then the circle was completely missed. (Case A) Continue on to the next trial in the for-loop.

	if (d >= 0) {
		// hits the circle:
		// get the first intersection:
		d = sqrt( d );
		float t1 = (-b + d ) / ( 2.*a );	// time to intersect the circle
		float t2 = (-b - d ) / ( 2.*a );	// time to intersect the circle
		float tmin = t1 < t2 ? t1 : t2;		// only care about the first intersection
	

		//If tmin is less than 0., then the circle completely engulfs the laser pointer. (Case B) Continue on to the next trial in the for-loop.
		if(tmin >= 0) {

			// where does it intersect the circle?
			float xcir = tmin;
			float ycir = tmin;

			// get the unitized normal vector at the point of intersection:
			float nx = xcir - xc;
			float ny = ycir - yc;
			float n = sqrt( nx*nx + ny*ny );
			nx /= n;	// unit vector
			ny /= n;	// unit vector

			// get the unitized incoming vector:
			float inx = xcir - 0.;
			float iny = ycir - 0.;
			float in = sqrt( inx*inx + iny*iny );
			inx /= in;	// unit vector
			iny /= in;	// unit vector

			// get the outgoing (bounced) vector:
			float dot = inx*nx + iny*ny;
			float outx = inx - 2.*nx*dot;	// angle of reflection = angle of incidence`
			float outy = iny - 2.*ny*dot;	// angle of reflection = angle of incidence`

			// find out if it hits the infinite plate:
			float t = ( 0. - ycir ) / outy;

			//If t is less than 0., then the reflected beam went up instead of down. Continue on to the next trial in the for-loop.
			//Otherwise, this beam hit the infinite plate. (Case D) Increment the number of hits and continue on to the next trial in the for-loop.
			if (t >= 0) {
				D[gid] = 1;
			}
			else {
				D[gid] = 0;
			}
		}
	}
}


// main program:

int
main( int argc, char* argv[ ] )
{
	//int dev = findCudaDevice(argc, (const char **)argv);


	const float XCMIN =	  0.;
	const float XCMAX =	 2.0;
	const float YCMIN =	  0.;
	const float YCMAX =	 2.0;
	const float RMIN  =	 0.5;
	const float RMAX  =	 2.0;

	// allocate host memory:

	float * hA = new float [ SIZE ];
	float * hB = new float [ SIZE ];
	float * hC = new float [ SIZE ];
	float * hD = new float [ SIZE ];

	for( int n = 0; n < SIZE; n++ )
     {       
     	hA[n] = chooseRand( XCMIN, XCMAX );
        hB[n] = chooseRand( YCMIN, YCMAX );
     	hC[n] = chooseRand(  RMIN,  RMAX ); 
     	hD[n] = 0.;
     }

	// allocate device memory:

	float *dA, *dB, *dC, *dD;

	dim3 dimsA( SIZE, 1, 1 );
	dim3 dimsB( SIZE, 1, 1 );
	dim3 dimsC( SIZE, 1, 1 );
	dim3 dimsD( SIZE, 1, 1 );

	//__shared__ float prods[SIZE/BLOCKSIZE];


	cudaError_t status;
	status = cudaMalloc( reinterpret_cast<void **>(&dA), SIZE*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dB), SIZE*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dC), SIZE*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dD), SIZE*sizeof(float) );
		checkCudaErrors( status );


	// copy host memory to the device:

	status = cudaMemcpy( dA, hA, SIZE*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
	status = cudaMemcpy( dB, hB, SIZE*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
	status = cudaMemcpy( dC, hC, SIZE*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
	status = cudaMemcpy( dD, hD, SIZE*sizeof(float), cudaMemcpyHostToDevice );

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( SIZE / threads.x, 1, 1 );

	// Create and start timer

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

	for( int t = 0; t < NUMTRIALS; t++)
	{
	        ArrayMul<<< grid, threads >>>( dA, dB, dC, dD );
	}

	// record the stop event:

	status = cudaEventRecord( stop, NULL );
		checkCudaErrors( status );

	// wait for the stop event to complete:

	status = cudaEventSynchronize( stop );
		checkCudaErrors( status );

	float msecTotal = 0.0f;
	status = cudaEventElapsedTime( &msecTotal, start, stop );
		checkCudaErrors( status );

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	double multsPerSecond = (float)SIZE * (float)NUMTRIALS / secondsTotal;
	double megaMultsPerSecond = multsPerSecond / 1000000.;
	//fprintf( stderr, "Array Size = %10d, MegaMultReductions/Second = %10.2lf\n", SIZE, megaMultsPerSecond );

	// copy result from the device to the host:

	status = cudaMemcpy( hC, dC, SIZE*sizeof(float), cudaMemcpyDeviceToHost );
		checkCudaErrors( status );

	status = cudaMemcpy( hD, dD, SIZE*sizeof(float), cudaMemcpyDeviceToHost );
		checkCudaErrors( status );

	double hits = 0;
	for ( int i = 0; i < SIZE; i++) {
		hits += hD[i];
	}	

	//fprintf(stderr, "Size: %d\n", SIZE);
	//double probability = hits/SIZE;
	fprintf(stderr, "Probability:%8.2lf\n", hits/SIZE);
	//printf("%d\t%d\t%10.2lf\t%8.2lf\n", BLOCKSIZE, SIZE, megaMultsPerSecond, hits/SIZE);
	printf("%10.2lf\t", megaMultsPerSecond);

	// clean up memory:
	delete [ ] hA;
	delete [ ] hB;
	delete [ ] hC;
	delete [ ] hD;

	status = cudaFree( dA );
		checkCudaErrors( status );
	status = cudaFree( dB );
		checkCudaErrors( status );
	status = cudaFree( dC );
		checkCudaErrors( status );
	status = cudaFree( dD );
		checkCudaErrors( status );
	

	return 0;
}

