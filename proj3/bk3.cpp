#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <omp.h>

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float WATER_FILLS_PER_MONTH =		4.0;
const float ONE_DEER_EATS_PER_MONTH =	1.0;
const float ONE_DEER_DRINKS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =				2.0;	// plus or minus noise

const float AVG_TEMP =					60.0;	// average
const float AMP_TEMP =					20.0;	// plus or minus
const float RANDOM_TEMP =				10.0;	// plus or minus noise

const float MIDTEMP =					40.0;
const float MIDPRECIP =					10.0;

unsigned int seed = 					0;

// Now variables
int 		StartNowYear = 				2020;
int 		EndYear = 					2026;
int			NowYear = 					StartNowYear;	// 2020 - 2025
int			NowMonth = 					0;				// 0 - 11
float		NowPrecip = 				AVG_PRECIP_PER_MONTH;				// inches of rain per month
float		NowTemp = 					AVG_TEMP;				// temperature this month
float		NowHeight = 				1;				// grain height in inches
int			NowNumDeer = 				1;				// number of deer in the current population
int 		NowWater = 					2;				// amount of water in the pond for the graindeer

// Function prototypes
float   Ranf( unsigned int*, float, float);
int 	Ranf( unsigned int, int, int);
float 	SQR(float x) ;
void 	computeEnvironemnt ();
void 	incMonth();
void 	Watcher();
void 	GrainDeer();
void 	Grain();
void 	Water();

int main(){
	#ifndef _OPENMP
    	std::fprintf(stderr, "OpenMP not supported, exiting program.\n");
    	return 1;
	#endif
// Calculate current environmental parameters
	computeEnvironemnt();

	omp_set_num_threads(4);	// same as # of sections
	#pragma omp parallel sections 
	{
		#pragma omp section 
		{
			GrainDeer();
		}

		#pragma omp section 
		{
			Grain();
		}

		#pragma omp section 
		{
			Watcher();
		}

		#pragma omp section
		{
			Water();
		}
	}
	return 0;
}

void Watcher(){
	while( NowYear < EndYear ){

	// DoneComputing barrier:
	#pragma omp barrier

	// DoneAssigning barrier:
	#pragma omp barrier

	// Print current set of global state variables
	if (NowYear == StartNowYear && NowMonth == 0)	
		printf("Curr Year \t Curr Month \t Curr Precip \t Curr Temp \t Curr Height \t Curr Graindeer \t Curr Water \n");
	printf("%d \t\t %d \t\t  %f \t  %f \t  %f \t  %d \t\t\t  %d \n", NowYear, NowMonth, NowPrecip, ((5. / 9.) * (NowTemp - 32)), (NowHeight * 2.54), NowNumDeer, NowWater);

	// Increment month count and compute new tempature and precipitation
	incMonth();
	computeEnvironemnt();
	// DonePrinting barrier:
	#pragma omp barrier
	}
}

void Water(){
	while( NowYear < EndYear ){
	// New amount of water
	float NextWater = NowWater;
	float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.));

	NextWater += precipFactor * WATER_FILLS_PER_MONTH;
	NextWater -= (float)NowNumDeer * ONE_DEER_DRINKS_PER_MONTH;
	if (NextWater < 0.)
		NextWater = 0.;

	// DoneComputing barrier:
	#pragma omp barrier

	// Set global variable to new value
	NowWater = NextWater;

	// DoneAssigning barrier:
	#pragma omp barrier

	// DonePrinting barrier:
	#pragma omp barrier
	}
}

void GrainDeer(){
	while( NowYear < EndYear ){
	// New number of GrainDeer after grain
		int lostOrGained = 0;
		int NextNumDeer;

		if(NowNumDeer > NowHeight)
			lostOrGained--;
		else
			lostOrGained++;
		
	// New number of GrainDeer after water
		if(NowNumDeer > NowWater){
			int diff = NowNumDeer - NowWater;
			lostOrGained -= diff;
		}

		NextNumDeer = NowNumDeer + lostOrGained;
		if (NextNumDeer < 0)
			NextNumDeer = 0;

	// DoneComputing barrier:
	#pragma omp barrier

	// Set global variable to new value
	NowNumDeer = NextNumDeer;

	// DoneAssigning barrier:
	#pragma omp barrier

	// DonePrinting barrier:
	#pragma omp barrier
	}
}

void Grain(){
	while( NowYear < EndYear ){
	// New number of Grain
	float NextHeight = NowHeight;
	float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.));
	float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.));
	
	NextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
 	NextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

	if(NextHeight < 0.)
		NextHeight = 0.;

	// DoneComputing barrier:
	#pragma omp barrier

	// Set global variable to new value
	NowHeight = NextHeight;

	// DoneAssigning barrier:
	#pragma omp barrier

	// DonePrinting barrier:
	#pragma omp barrier
	}
}

void incMonth(){
	if (++NowMonth > 11){
		NowMonth = 0;
		NowYear++;
	}
}

void computeEnvironemnt (){
	float ang = (30.*(float)NowMonth + 15.) * ( M_PI / 180.);

	float temp = AVG_TEMP - AMP_TEMP * cos(ang);
	NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
	NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);

	if(NowPrecip < 0.)
		NowPrecip = 0.;
}

float Ranf( unsigned int *seedp,  float low, float high ){
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int Ranf( unsigned int *seedp, int ilow, int ihigh ){
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

float SQR( float x ){
        return x*x;
}