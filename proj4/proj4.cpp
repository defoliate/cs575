//#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <xmmintrin.h>


#ifndef ARRAYSIZE
#define ARRAYSIZE 100
#endif

#ifndef NUMTRIES
#define NUMTRIES 1
#endif

#ifndef SSE_WIDTH
#define SSE_WIDTH 4
#endif

float SimdMulSum(float*, float*, int);
float StdMulSum(float*, float*, int); 

int main(int argc, char *argv[ ]) {
    #ifndef _OPENMP
    fprintf( stderr, "No OpenMP support!\n" );
    return 1;
    #endif

    double maxMultsSIMD, maxMultsStd, multsPerSec;
    float SimdSum, StdSum;
    double time0, time1;
      
    float* a = (float*)malloc(ARRAYSIZE*sizeof(float));
    float* b = (float*)malloc(ARRAYSIZE*sizeof(float));
    for(int i = 0; i < ARRAYSIZE; i++) {
        a[i] = 1;
        b[i] = 2;
    }

    maxMultsSIMD = maxMultsStd = 0.;
    for(int i = 0; i < NUMTRIES; i++) {
        time0 = omp_get_wtime( );
        SimdSum = SimdMulSum(a, b, ARRAYSIZE);
        time1 = omp_get_wtime( );

        multsPerSec = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
        if(multsPerSec > maxMultsSIMD) {
            maxMultsSIMD = multsPerSec;
        }

        time0 = omp_get_wtime( );
        StdSum = StdMulSum(a, b, ARRAYSIZE);
        time1 = omp_get_wtime( );

        multsPerSec = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
        if(multsPerSec > maxMultsStd) {
            maxMultsStd = multsPerSec;
        }
        //printf("SIMD SUM:%0.2lf STD SUM: %0.2lf\n", SimdSum, StdSum);
        fprintf(stderr, "%0.2lf\n", SimdSum + StdSum);
    }

    printf("%d, %0.2lf, %0.2lf\n", ARRAYSIZE, maxMultsSIMD, maxMultsStd);
 
    free(a);
    free(b);
 
    return 0;

}

float SimdMulSum(float *a, float *b, int len) {
    float sum[4] = {0., 0., 0., 0.};
    int limit = (len/SSE_WIDTH) * SSE_WIDTH;
    register float *pa = a;
    register float *pb = b;

    __m128 ss = _mm_loadu_ps(&sum[0]);
    for(int i = 0; i < limit; i += SSE_WIDTH) {
        ss = _mm_add_ps(ss, _mm_mul_ps(_mm_loadu_ps(pa), _mm_loadu_ps(pb)));
        pa += SSE_WIDTH;
        pb += SSE_WIDTH;
    }
    _mm_storeu_ps(&sum[0], ss);

    // remaining indices after last multi of sse_width.
    for(int i = limit; i < len; i++) {
        sum[0] += a[i] * b[i];
    }
    
    return sum[0] + sum[1] + sum[2] + sum[3];
}

float StdMulSum(float *a, float *b, int len) {
    float sum = 0.;
    for(int i = 0; i < len; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}
