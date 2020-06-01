#include <omp.h>
#include <stdio.h>
#include <math.h>

#ifndef NUMTA
#define NUMTA 1
#endif

#ifndef NUMT
#define NUMT	         4
#endif

#define SIZE       	60000	// you decide
#define NUMTRIES        500	 // you decide

float A[SIZE];
float B[SIZE];
float C[SIZE];

int
main()
{
#ifndef _OPENMP
    fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
    return 1;
#endif

    // inialize the arrays:
    for (int i = 0; i < SIZE; i++)
    {
        A[i] = 1.;
        B[i] = 2.;
    }

    omp_set_num_threads(NUMT);
    fprintf(stderr, "Using %d threads\n", NUMT);

    double maxMegaMults = 0.;

    for (int t = 0; t < NUMTRIES; t++)
    {
        double time0 = omp_get_wtime();
        int i;
#pragma omp parallel for private  (i)
        for (i = 0; i < SIZE; i++)
        {
            C[i] = A[i] * B[i];
        }

        double time1 = omp_get_wtime();
        double megaMults = (double)SIZE / (time1 - time0) / 1000000.;
        if (megaMults > maxMegaMults)
            maxMegaMults = megaMults;
    }

    printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
    omp_set_num_threads(NUMTA);
    fprintf(stderr, "Using %d threads\n", NUMTA);

    double maxMegaMultsA = 0.;

    for (int t = 0; t < NUMTRIES; t++)
    {
        double time0 = omp_get_wtime();
        int i;
#pragma omp parallel for private  (i)
        for (i = 0; i < SIZE; i++)
        {
            C[i] = A[i] * B[i];
        }

        double time1 = omp_get_wtime();
        double megaMultsA = (double)SIZE / (time1 - time0) / 1000000.;
        if (megaMultsA > maxMegaMultsA)
            maxMegaMultsA = megaMultsA;
    }

    printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMultsA);

    float S = maxMegaMults/ maxMegaMultsA;
    float Fp = (4. / 3.) * (1. - (1. / S));
    printf("S=%f,Fp=%f\n", S, Fp);

    // note: %lf stands for "long float", which is how printf prints a "double"
    //        %d stands for "decimal integer", not "double"

    return 0;
}
