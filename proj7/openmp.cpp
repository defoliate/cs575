#include <cstdlib>
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>

void autoCorrelate(float *array, float *sums, int size, int threads);

int main() {

    FILE *fp = fopen("signal.txt", "r");

    if(!fp){
        fprintf(stderr, "Error opening file\n");
    }

    int size;
    fscanf(fp, "%d", &size);

    float *array = new float[2 * size];
    float *sums  = new float[size];
    for(int i = 0; i < size; i++) {
    	fscanf(fp, "%f", &array[i]);
	    array[i + size] = array[i];
    }
    fclose(fp);

    //for(int i = 0; i < size; i++){
      //  fprintf(stderr, "%8.2lf\n", array[i]);
    //}

    
    autoCorrelate(array, sums, size, 1);
    printf("\n\n\n\n\n");
    autoCorrelate(array, sums, size, 16);
    
    return 0;
}


void autoCorrelate(float *array, float *sums, int size, int threads) {

    omp_set_num_threads(threads);

    int loops = 10;
    double averageTime = 0;
    double totalTime = 0;
    double maxPerformance = 1000;

    for(int i = 0; i < loops; i++) {
        
        double t_not = omp_get_wtime();

        #pragma omp parallel for default(none) shared(size, array, sums)
        for(int shift = 0; shift < size; shift++) {
	        float sum = 0.;
	        for(int i = 0; i < size; i++) {
		        sum += array[i] * array[i + shift];
	        }
	        sums[shift] = sum;
        }

        double t = omp_get_wtime();
        if(t - t_not < maxPerformance){
            maxPerformance = t - t_not;
        }

        totalTime += t - t_not;
        
    }
    
    printf("%d\t%8.2lf\n\n", threads,(double)size * size / maxPerformance / 1000000);


    printf("Index\tSum\n");
    for(int i = 0; i < 512; i++){
        printf("%d\t%8.2f\n", i+1, sums[i+1]);
    }
}