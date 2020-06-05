#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
// setting the number of threads:
#ifndef NUMT
#define NUMT		1
#endif


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
omp_set_num_threads( NUMT );
double time0 = omp_get_wtime( );
#pragma omp parallel for default(none) shared(A,Sums,Size)
for( int shift = 0; shift < Size; shift++ )
{
	float sum = 0.;
	for( int i = 0; i < Size; i++ )
	{
		sum += A[i] * A[i + shift];
	}
	Sums[shift] = sum;	// note the "fix #2" from false sharing if you are using OpenMP
}
double time1 = omp_get_wtime( );
double Performance = 2*(double)Size*(double)Size/ ( time1 - time0 ) / 1000000.;
fprintf( stderr,"The number of threads is %8.2d\n",NUMT);
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