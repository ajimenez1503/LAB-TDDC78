#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"

int main (int argc, char ** argv) {
    int xsize, ysize, colmax,radius;
    pixel * src= (pixel *) calloc ( MAX_PIXELS, sizeof(pixel) );
    double * w= (double *) calloc ( MAX_RAD+1, sizeof(double) );
    struct timespec stime, etime;

    thread_data thread_data_array[NUM_THREADS];//data for every thread
	pthread_t threads[NUM_THREADS];//array of thread


    /* Take care of the arguments */

    if (argc != 4) {
    	fprintf(stderr, "Usage: %s radius infile outfile\n", argv[0]);
    	exit(1);
    }
    radius = atoi(argv[1]);
    if((radius > MAX_RAD) || (radius < 1)) {
    	fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
    	exit(1);
    }

    /* read file */
    if(read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0){
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }
    if (colmax > 255) {
    	fprintf(stderr, "Too large maximum color-component value\n");
    	exit(1);
    }
    printf("Has read the image, generating coefficients\n");

    /* filter */
    get_gauss_weights(radius, w);

    printf("Calling filter \n");

    //calculate data for thread_data
    int lsize=(xsize*ysize)/NUM_THREADS;
    int lysize=ysize/NUM_THREADS;//number of row (of xsize pixel) per thread.
    int i=0;
    for(i=0;i<NUM_THREADS;i++){
		thread_data_array[i].src_local = src;
        thread_data_array[i].ybegin = lysize*i;
        thread_data_array[i].yend = lysize*(i+1);
        thread_data_array[i].xsize=xsize;
		thread_data_array[i].ysize=ysize;
        thread_data_array[i].w = w;
        thread_data_array[i].radius=radius;
	}
    //init dst filter
    initDst(xsize,ysize);
    clock_gettime(CLOCK_REALTIME, &stime);

    for(i=0;i<NUM_THREADS;i++){
		//printf("In main: creating thread %d\n", i);
		if (pthread_create(&threads[i], NULL, blurfilter, (void *)&thread_data_array[i])){
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

    /* write result */
    //printf("Writing output file\n");

    if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0){
        fprintf(stderr, "Error writing file\n");
        exit(1);
    }

    free (w);
    free (src);
    FreeDst();
    return(0);
}
