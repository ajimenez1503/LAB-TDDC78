#include <mpi.h>
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
    struct timespec stime, etime;

    int rank, np;
    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &np);        /* get number of processes */
    MPI_Comm com = MPI_COMM_WORLD;

    /* Take care of the arguments */

    if (rank==0){
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
        //printf("Has read the image, calling filter\n");
        clock_gettime(CLOCK_REALTIME, &stime);
    }
    thresfilter(size, src_total);
    if (rank==0){
        clock_gettime(CLOCK_REALTIME, &etime);
        printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

        /* write result */
        //printf("Writing output file\n");

        if(write_ppm (argv[2], xsize, ysize, (char *)src_total) != 0){
        	fprintf(stderr, "Error writing file\n");
        	exit(1);
        }
    }
    free (src_total);
    MPI_Finalize();
    return(0);
}
