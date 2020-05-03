#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
using namespace std;
int	NowYear  = 2020;		// 2020 - 2025
int	NowMonth = 2;		// 1 - 12 we begin with 2 since data for 1 is already known

unsigned int seed  = 0;
float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight  = 30;		// grain height in inches
int	    NowNumDeer = 20;		// number of deer in the current population
int     NowNumWolf = 2;

float NextHeight;
int NextNumDeer;
int NextNumWolf;
const float GRAIN_GROWS_PER_MONTH =	  9.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH =	  7.0;	// average
const float AMP_PRECIP_PER_MONTH =	  6.0;	// plus or minus
const float RANDOM_PRECIP =			  2.0;	// plus or minus noise

const float AVG_TEMP =				  60.0;	// average
const float AMP_TEMP =				  20.0;	// plus or minus
const float RANDOM_TEMP =			  10.0;	// plus or minus noise

const float MIDTEMP =				  40.0;
const float MIDPRECIP =				  10.0;
float ang;
float temp;
float precip;
float tempFactor;
float precipFactor;
ofstream FileTemp;
ofstream FilePrecip;
ofstream FileGrain;
ofstream FileDeer;
ofstream FileWolf;
// function prototypes:
float Ranf( unsigned int *seedp,  float low, float high );
int Ranf( unsigned int *seedp, int ilow, int ihigh );
float SQR( float x );

void GrainDeer();
void DeerWolf();
void Grain();
void Watcher();

// main program:
int main( int argc, char *argv[ ] )
{
	#ifndef _OPENMP
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif
	
	// We initialize the temperature and precipation variables
	ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
		{NowPrecip = 0.;}
	tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );

	precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
	// Let us begin the recoding of the simulation.
	FileTemp.open("FTemp.m");
	FileTemp << "Temp = ["<<NowTemp;

	FilePrecip.open("FPrecip.m");
	FilePrecip << "Precip = ["<<NowPrecip;

	FileGrain.open("FGrain.m");
	FileGrain << "Grain = ["<<NowHeight;

	FileDeer.open("FDeer.m");
	FileDeer << "Deer = ["<<NowNumDeer;

	FileWolf.open("FWolf.m");
	FileWolf << "Wolf = ["<<NowNumWolf;
	omp_set_num_threads( 4 );	// same as # of sections
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer( );
		}

		#pragma omp section
		{
			Grain( );
		}

		#pragma omp section
		{
			DeerWolf( );
		}

		#pragma omp section
		{
			Watcher( );
		}
	}
	FileTemp<<"];";
	FileTemp.close();
	FilePrecip<<"];";
	FilePrecip.close();
	FileGrain<<"];";
	FileGrain.close();
	FileDeer<<"];";
	FileDeer.close();
	FileWolf<<"];";
	FileWolf.close();
}       
void 
Grain()
{
	while( NowYear < 2026 )
	{
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation:
	NextHeight  = NowHeight;
 	NextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
 	NextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
	if (NextHeight<=0)
		{NextHeight = 0;}
	// DoneComputing barrier:
	#pragma omp barrier
	NowHeight = NextHeight;
	// DoneAssigning barrier:
	#pragma omp barrier
	FileGrain<<","<<NowHeight;
	// DonePrinting barrier:
	#pragma omp barrier
	}
}
void 
GrainDeer()
{
	while( NowYear < 2026 )
	{
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation:
	if (NowHeight < NowNumDeer)
		{NextNumDeer = NowNumDeer-1;}
	else
		{NextNumDeer = NextNumDeer+1;}
	NextNumDeer = NextNumDeer -(int)floor(NowNumWolf/3);
	if (NextNumDeer <= 0)
		{NextNumDeer = 0;}
	// DoneComputing barrier:
	#pragma omp barrier
	NowNumDeer = NextNumDeer;
	// DoneAssigning barrier:
	#pragma omp barrier
	FileDeer<<","<<NowNumDeer;
	// DonePrinting barrier:
	#pragma omp barrier
	}
}
void
DeerWolf()
{
	while( NowYear < 2026 )
	{
	// compute a temporary next-value for this quantity
	// based on the current state of the simulation:
	if(NowNumWolf < NowNumDeer)
		{NextNumWolf = NowNumWolf + 1;}
	else
		{NextNumWolf = NowNumWolf - 1;}
	if(NextNumWolf <= 0)
		{NextNumWolf = 0;}
	// DoneComputing barrier:
	#pragma omp barrier
	NowNumWolf = NextNumWolf;
	// DoneAssigning barrier:
	#pragma omp barrier
	FileWolf<<","<<NowNumWolf;
	// DonePrinting barrier:
	#pragma omp barrier
	}
}

void
Watcher()
{
	while( NowYear < 2026 )
	{
	// DoneComputing barrier:
	#pragma omp barrier
	// DoneAssigning barrier:
	#pragma omp barrier
	// Update the environmental variables.
	if (NowMonth == 12)
		{NowYear++;
		 NowMonth = 1;}
	else
		{NowMonth++;}
	// cout<< NowMonth<<"\n"<<NowYear<<"\n"; //Debuging tool.
	ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip  < 0. )
		{NowPrecip = 0.;}
	tempFactor   = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
	precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
	// Printing the variables
	FileTemp  <<","<<NowTemp;
	FilePrecip<<","<<NowPrecip;
	// DonePrinting barrier:
	#pragma omp barrier
	}
}
float
SQR( float x )
{
        return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}