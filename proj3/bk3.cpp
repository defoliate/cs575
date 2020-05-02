#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifndef NUMT
#define NUMT	        4
#endif

void InitBarrier(int);
void WaitBarrier();
float Ranf(unsigned int *seedp, float low, float high);
int Ranf(unsigned int *seedp, int ilow, int ihigh);
float SQR(float x);

void GrainDeer();
void Grain();
void Watcher();
void ExtremeWeather();


int	NowYear;		// 2019 - 2024
int	NowMonth;		// 0 - 11
int outputMonth;	// month variable doesn't get reset for ouput purposes

float NowPrecip;	// inches of rain per month
float NowTemp;		// temperature this month
float NowHeight;	// grain height in inches
int	NowNumDeer;		// number of deer in the current population
float NowExtremeWeatherEffect; //multiplier that effects grain height due to extreme hot or cold conditions
float NowHeightEffect; //will store the effect on the grain height due extreme condition

omp_lock_t Lock;
int	NumInThreadTeam;
int	NumAtBarrier;
int	NumGone;
unsigned int seed = 0;

const float GRAIN_GROWS_PER_MONTH =	8.0;
const float ONE_DEER_EATS_PER_MONTH = 0.3;

const float AVG_PRECIP_PER_MONTH = 6.0;		// average
const float AMP_PRECIP_PER_MONTH = 6.0;		// plus or minus
const float RANDOM_PRECIP = 2.0;			// plus or minus noise
	
const float AVG_TEMP = 50.0;				// average
const float AMP_TEMP = 20.0;				// plus or minus
const float RANDOM_TEMP = 10.0;				// plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP =	10.0;



int main(int argc, char *argv[]) 
{
	// starting values
	NowMonth =   0;
  	NowYear =   2019;
  	NowNumDeer =   1;
  	NowHeight =   1.;
  	NowExtremeWeatherEffect = 0.;	//months since last fire
  	NowHeightEffect = 0.;

  	outputMonth = 1;

  	float ang0 = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang0 );
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float init_precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang0 );
	NowPrecip = init_precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
	{
		NowPrecip = 0.;
	}

  	printf("Month\tHeight\tDeer\tTemp\tRain\n");

	omp_set_num_threads(NUMT);	// same as # of sections


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
			ExtremeWeather();	// your own
		}
	
	}  

}


void GrainDeer() 
{
	while(NowYear < 2025)
	{
		float height = NowHeight;
		int deer = NowNumDeer;

		if(deer > height)
		{
			deer--;
		}
		else if(deer < height)
		{
			deer++;
		}

		//DoneComputing barrier
		#pragma omp barrier

		NowNumDeer = deer;

		//DoneAssigning barrier
		#pragma omp barrier

		//wait for watcher to print variables
		//DonePrinting barrier;
		#pragma omp barrier
	}
}


void Grain()
{
	while(NowYear < 2025)
	{
		float height = NowHeight;
		float tempFactor = exp(-SQR((NowTemp - MIDTEMP ) / 10.));
		float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP ) / 10.));
		float extremeMultiplier = NowExtremeWeatherEffect;
		float heightEffect = 0;


		height += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		height -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;


		if(height < 0)
		{
			height = 0;
		}

		float tempHeight = height;
		if((float)extremeMultiplier > 0.)
		{
			tempHeight *= extremeMultiplier;
		}

		heightEffect = height - tempHeight;
		height = tempHeight;

		//DoneComputing barrier
		#pragma omp barrier

		//set new grain height and effect
		NowHeight = height;

		//grain effect
		NowHeightEffect = heightEffect;

		//DoneAssigning barrier
		#pragma omp barrier


		//DonePrinting barrier
		#pragma omp barrier
	}

}



void ExtremeWeather()
{
	while(NowYear < 2025)
	{
		float currentTemp = NowTemp;
		float currentRain = NowPrecip;
		float extremeMultiplier = 0.;
		


		//trigger a fire
		if(currentTemp > 70.0 && currentRain < 10.0)
		{
			extremeMultiplier = 0.25;
		} 
		//trigger a frost
		else if(currentTemp < 4.0 && currentRain > 12.0) {
			extremeMultiplier = 0.25;
		}

		else if(currentTemp < 40.)
		{
			extremeMultiplier = .8;
		}

		else if(currentTemp < 35.)
		{
			extremeMultiplier = .7;
		}

		else if(currentTemp < 32.)
		{
			extremeMultiplier = .5;
		}

		
		//DoneComputing barrier
		#pragma omp barrier

		
		NowExtremeWeatherEffect = extremeMultiplier;

		//DoneAssigning barrier
		#pragma omp barrier

		//DonePrinting barrier
		#pragma omp barrier
	}
}


void Watcher()
{
	while(NowYear < 2025)
	{
		//DoneComputing barrier
		#pragma omp barrier

		//DoneAssigning barrier
		#pragma omp barrier

		//convert values to cm and celsius
		float outputHeight = NowHeight * 2.54;
		float outputPrecip = NowPrecip * 2.54;
		float outputTemp = (5./9.) * (NowTemp - 32); 
		float outputHeightEffect = NowHeightEffect * 2.54;

		//Print out all variables
		printf("%d\t%8.2lf\t%d\t%8.2lf\t%8.2lf\t%8.2lf\n", outputMonth, outputHeight, NowNumDeer, outputTemp, outputPrecip, outputHeightEffect);

		outputMonth++;

		//update date
		if(NowMonth == 11)
		{
			NowMonth = 0;
			NowYear++;
		}
		else 
		{
			NowMonth++;
		}


		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. )
			NowPrecip = 0.;



		//DonePrinting barrier
		#pragma omp barrier
	}
}


void InitBarrier( int n )
{
	NumInThreadTeam = n;
	NumAtBarrier = 0;
	omp_init_lock( &Lock );
}


void WaitBarrier()
{
	omp_set_lock( &Lock );
	{
    	NumAtBarrier++;
    	if( NumAtBarrier == NumInThreadTeam )
        {
        	NumGone = 0;
    	    NumAtBarrier = 0;
        	// let all other threads get back to what they were doing
			// before this one unlocks, knowing that they might immediately
			// call WaitBarrier( ) again:
        	while( NumGone != NumInThreadTeam-1 );
         	omp_unset_lock( &Lock );
           	return;
    	}
	}
	omp_unset_lock(&Lock);

	while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

	#pragma omp atomic
	NumGone++;			// this flags how many threads have returned
}


float Ranf(unsigned int *seedp, float low, float high)
{
	float r = (float) rand_r(seedp);              // 0 - RAND_MAX

	return(low + r * (high - low) / (float)RAND_MAX);
}


int Ranf(unsigned int *seedp, int ilow, int ihigh)
{
	float low = (float)ilow;
	float high = (float)ihigh + 0.9999f;

	return (int)(  Ranf(seedp, low,high) );
}

float SQR(float x)
{
        return x*x;
}