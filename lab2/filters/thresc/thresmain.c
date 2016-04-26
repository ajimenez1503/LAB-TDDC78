#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "ppmio.h"
#include "thresfilter.h"



int main (int argc, char ** argv) {
    int xsize=0, ysize=0, colmax=0,size=0;
    pixel * src_total;
    src_total= (pixel *) calloc ( MAX_PIXELS ,sizeof(pixel));
    thread_data thread_data_array[NUM_THREADS];//data for every thread
	pthread_t threads[NUM_THREADS];//array of thread
    struct timespec stime, etime;

    /* Take care of the arguments */

    if (argc != 3) {
    	fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
    	exit(1);
    }

    /* read file */
    if(read_ppm (argv[1], &xsize, &ysize, &colmax, (char *) src_total) != 0){
    	fprintf(stderr, "Error reading file\n");
    	exit(1);
    }
    size=xsize*ysize;
    if (colmax > 255) {
    	fprintf(stderr, "Too large maximum color-component value\n");
    	exit(1);
    }

    //calculate data for thread_data
    int lsize=size/NUM_THREADS;
    int i=0;
    for(i=0;i<NUM_THREADS;i++){
		thread_data_array[i].src_local = src_total;
		thread_data_array[i].sum=0;
        thread_data_array[i].begin = lsize*i;
		thread_data_array[i].end=lsize*(i+1);
	}
    //printf("Has read the image, calling filter\n");
    clock_gettime(CLOCK_REALTIME, &stime);
    //thresfilter(size, src_total);
    for(i=0;i<NUM_THREADS;i++){
		//printf("In main: creating thread %d\n", i);
		if (pthread_create(&threads[i], NULL, thresfilter, (void *)&thread_data_array[i])){
			printf("ERROR; return code from pthread_create() is \n");
			exit(-1);
		}
	}
    // wait for  thread to finish
	for(i=0;i<NUM_THREADS;i++){
		if(pthread_join(threads[i], NULL)) {
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}
	}
    clock_gettime(CLOCK_REALTIME, &etime);
    printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

    // write result
    //printf("Writing output file\n");

    if(write_ppm (argv[2], xsize, ysize, (char *)src_total) != 0){
    	fprintf(stderr, "Error writing file\n");
    	exit(1);
    }
    free (src_total);
    return(0);
}
