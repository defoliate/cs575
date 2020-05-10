#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <xmmintrin.h>


#ifndef NUMT
#define NUMT 4
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE 100
#endif

#ifndef NUMTRIES
#define NUMTRIES 1
#endif

#ifndef SSE_WIDTH
#define SSE_WIDTH 4
#endif

#define NUM_ELEMENTS_PER_CORE ARRAYSIZE / NUMT


float SimdMulSum(float*, float*, int);
float StdMulSum(float*, float*, int); 


int main(int argc, char *argv[ ]) {
    #ifndef _OPENMP
    fprintf( stderr, "No OpenMP support!\n" );
    return 1;
    #endif

    double maxMultsSIMD, maxMultsSIMDT, maxMultsStd, maxMultsStdT, multsPerSec;
    float SimdSum, SimdTSum, StdSum, StdTSum;
    double time0, time1;
      
    float* a = (float*)malloc(ARRAYSIZE*sizeof(float));
    float* b = (float*)malloc(ARRAYSIZE*sizeof(float));
    for(int i = 0; i < ARRAYSIZE; i++) {
        a[i] = 1;
        b[i] = 2;
    }

    omp_set_num_threads(NUMT);

    // Run each performance evaluation NUMTRIES times.
    maxMultsSIMD = maxMultsStd = maxMultsSIMDT = maxMultsStdT = 0.;
    for(int i = 0; i < NUMTRIES; i++) {
        // Test SIMD.
        time0 = omp_get_wtime( );
        SimdSum = SimdMulSum(a, b, ARRAYSIZE);
        time1 = omp_get_wtime( );

        multsPerSec = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
        if(multsPerSec > maxMultsSIMD) {
            maxMultsSIMD = multsPerSec;
        }

        // Test Standard Loop.
        time0 = omp_get_wtime( );
        StdSum = StdMulSum(a, b, ARRAYSIZE);
        time1 = omp_get_wtime( );

        multsPerSec = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
        if(multsPerSec > maxMultsStd) {
            maxMultsStd = multsPerSec;
        }

        // Test SIMD with Threading.
        SimdTSum = 0.;
        time0 = omp_get_wtime( );
        //using reduction
        #pragma omp parallel reduction(+:SimdTSum)
        {
            int first = omp_get_thread_num() * NUM_ELEMENTS_PER_CORE;
            SimdTSum += SimdMulSum(&a[first], &b[first], NUM_ELEMENTS_PER_CORE);
        }
        time1 = omp_get_wtime( );

        multsPerSec = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
        if(multsPerSec > maxMultsSIMDT) {
            maxMultsSIMDT = multsPerSec;
        }

        // Test Standard Multithreading.
        StdTSum = 0.;
        time0 = omp_get_wtime( );
        #pragma omp parallel reduction(+:StdTSum)
        {
            int first = omp_get_thread_num() * NUM_ELEMENTS_PER_CORE;
            StdTSum += StdMulSum(&a[first], &b[first], NUM_ELEMENTS_PER_CORE);
        }
        time1 = omp_get_wtime( );

        multsPerSec = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
        if(multsPerSec > maxMultsStdT) {
            maxMultsStdT = multsPerSec;
        }

        // Print without outputting to script.
        fprintf(stderr, "%0.2lf, %0.2lf, %0.2lf, %0.2lf\n", SimdSum, StdSum, SimdTSum, StdTSum);
    }

    printf("%d, %0.2lf, %0.2lf, ", ARRAYSIZE, maxMultsSIMD, maxMultsStd);
    printf("%0.2lf, %0.2lf\n", maxMultsSIMDT, maxMultsStdT);

    free(a);
    free(b);
 
    return 0;

}

float SimdMulSum1(float *a, float *b, int len) {
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

float StdMulSum1(float *a, float *b, int len) {
    float sum = 0.;
    for(int i = 0; i < len; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}
void
SimdMul( float *a, float *b, float *c, int len )
{
    int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
    __asm
    (
        ".att_syntax\n\t"
"movq -24(%rbp), %r8\n\t" // a
"movq -32(%rbp), %rcx\n\t" // b
"movq -40(%rbp), %rdx\n\t" // c
);
    for( int i = 0; i < limit; i += SSE_WIDTH )
    {
        __asm
        (
            ".att_syntax\n\t"
"movups (%r8), %xmm0\n\t" // load the first sse register
"movups (%rcx), %xmm1\n\t" // load the second sse register
"mulps %xmm1, %xmm0\n\t" // do the multiply
"movups %xmm0, (%rdx)\n\t" // store the result
"addq $16, %r8\n\t"
"addq $16, %rcx\n\t"
"addq $16, %rdx\n\t"
);
    }
    for( int i = limit; i < len; i++ )
    {
        c[ i ] = a[ i ] * b[ i ];
    }
}
float
SimdMulSum( float *a, float *b, int len )
{
    float sum[4] = { 0., 0., 0., 0. };
    int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
    __asm
    (
        ".att_syntax\n\t"
"movq -40(%rbp), %r8\n\t" // a
"movq -48(%rbp), %rcx\n\t" // b
"leaq -32(%rbp), %rdx\n\t" // &sum[0]
"movups (%rdx), %xmm2\n\t" // 4 copies of 0. in xmm2
);
    for( int i = 0; i < limit; i += SSE_WIDTH )
    {
        __asm
        (
            ".att_syntax\n\t"
"movups (%r8), %xmm0\n\t" // load the first sse register
"movups (%rcx), %xmm1\n\t" // load the second sse register
"mulps %xmm1, %xmm0\n\t" // do the multiply
"addps %xmm0, %xmm2\n\t" // do the add
"addq $16, %r8\n\t"
"addq $16, %rcx\n\t"
);
    }
    __asm
    (
        ".att_syntax\n\t"
"movups %xmm2, (%rdx)\n\t" // copy the sums back to sum[ ]
);
    for( int i = limit; i < len; i++ )
    {
        sum[0] += a[ i ] * b[ i ];
    }
    return sum[0] + sum[1] + sum[2] + sum[3];
}