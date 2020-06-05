#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <xmmintrin.h>
// function prototypes:
#ifndef SSE_WIDTH
#define SSE_WIDTH		4
#endif

float
SimdMulSum( float *a, float *b, int len );

int
main( int argc, char *argv[ ] )
{
	#ifndef _OPENMP
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif
// Reading Data from file

FILE *fp = fopen( "signal.txt", "r" );
if( fp == NULL )
{
	fprintf( stderr, "Cannot open file 'signal.txt'\n" );
	exit( 1 );
}

int Size;
fscanf( fp, "%d", &Size );
float *A =     new float[ 2*Size ];
float *Sums  = new float[ 1*Size ];
for( int i = 0; i < Size; i++ )
{
	fscanf( fp, "%f", &A[i] );
	A[i+Size] = A[i];		// duplicate the array
}
fclose( fp );

//Setting up OpenMP
//omp_set_num_threads( NUMT );
double time0 = omp_get_wtime( );
for( int shift = 0; shift < Size; shift++ )
{
	Sums[shift] = Sums[shift] = SimdMulSum( &A[0], &A[0+shift], Size );
}
double time1 = omp_get_wtime( );
double Performance = 2*(double)Size*(double)Size/ ( time1 - time0 ) / 1000000.;
//fprintf( stderr,"The number of threads is %8.2d\n",NUMT);
fprintf( stderr,"Peak Performance = %8.2lf MegaOperations/Sec\n", Performance );

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

float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] );
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}