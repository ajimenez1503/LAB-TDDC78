#include <mpi.h>
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
    double * w= (double *) calloc ( MAX_RAD+1, sizeof(pixel) );
    struct timespec stime, etime;


    int rank, np;
    MPI_Comm com = MPI_COMM_WORLD;
    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (com, &rank);        /* get current process id */
    MPI_Comm_size (com, &np);        /* get number of processes */
    //printf("id %d of %d\n",rank,np);

    /* Take care of the arguments */

    if (rank==0){

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

        clock_gettime(CLOCK_REALTIME, &stime);
    }
    blurfilter(xsize, ysize, src, radius, w);
    if (rank==0){
        clock_gettime(CLOCK_REALTIME, &etime);

        printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

        /* write result */
        printf("Writing output file\n");

        if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0){
            fprintf(stderr, "Error writing file\n");
            exit(1);
        }

    }
    free (src);
    free (w);
    MPI_Finalize();
    return(0);
}
