#include <cstdlib>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include "simd.p4.h"


#define SIZE 32768
float array[2 * SIZE];
float sums[SIZE];

int main() {

    FILE *fp = fopen("signal.txt", "r");

    if(!fp){
        fprintf(stderr, "Failed to open file\n");
        exit(1);
    }

    int size;
    fscanf(fp, "%d", &size);
    
    for(int i = 0; i < size; i++) {
    	fscanf(fp, "%f", &array[i]);
	    array[i + size] = array[i];
    }
    fclose(fp);

    int loops = 10;
    double averageTime = 0;
    double totalTime = 0;
    double maxPerformance = 1000;
    double t_not, t;

	for(int i = 0; i < loops; i++) {

		t_not = omp_get_wtime();

        for(int shift = 0; shift < size; shift++) {
		    sums[shift] = SimdMulSum(&array[0], &array[0+shift], size);
        }

		t = omp_get_wtime();
		
		totalTime += t - t_not;

		if(t - t_not < maxPerformance){
			maxPerformance = t - t_not; 
        }
	}

    
    printf("Max Performance: %8.2lf MegaShifts/s\n", (double)size * size / maxPerformance / 1000000);

    printf("Index\tSum\n");
    for(int i = 0; i < 512; i++){
        printf("%d\t%8.2lf\n", i+1, sums[i+1]);
    }

    return 0;
}
