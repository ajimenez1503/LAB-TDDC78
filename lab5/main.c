#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "definitions.h"
#include "coordinate.h"
#include "physics.h"
#include "matrix.h"
#include <time.h>       /* time */

int main (int argc, char ** argv) {
    int rank, np;
    MPI_Init (&argc, &argv);      // starts MPI
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        // get current process id
    MPI_Comm_size (MPI_COMM_WORLD, &np);       // get number of processes
    MPI_Comm com = MPI_COMM_WORLD;
    printf("ID= %d of number of processors %d\n",rank,np );

    printf("ID= %d Init initialize \n",rank );
    srand(time(NULL) + rank); //use current time as seed for random generator
    int xsize=10, ysize=10;
    struct Matrix m = CreateNewMatrix(xsize,ysize);//x column and y row
    struct part_cord aux_part;
    float r=0, omega=0;
    int y=0,x=0;
    for (x=0; x<xsize; x++) {
        for (y=0; y<ysize; y++) {
            aux_part.y=y;
            aux_part.x=x;
            r=rand()*MAX_INITIAL_VELOCITY;
            omega=rand()*2*PI;
            aux_part.vx=r*cos(omega);
            aux_part.vy=r*sin(omega);
            SetMatrixValue(&m,x,y,aux_part);
            //printf("ID= %d  initialize (%d,%d) \n",rank,x,y );
        }
    }
    //printf("ID= %d finish initialize \n",rank );


    printf("ID= %d Init check collision \n",rank );
    int time_step=0;
    for(time_step=0; time_step<=3600;time_step++){//1 min of time_step
        for (x=0; x<xsize; x++) {
            for (y=0; y<ysize; y++) {
                if(y<ysize-1 && x<xsize-1){
                    if(collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x,y+1))==1 ||
                    collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x+1,y))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,y),time_step) ;
                    }
                }
                else if(x<xsize-1){
                    if(collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x+1,y))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,y),time_step) ;
                    }
                }else if(y<ysize-1){
                    if(collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x,y+1))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,y),time_step) ;
                    }
                }
            }
        }
    }
    //printf("ID= %d finish check collision \n",rank );


    float totalmomentum=0;
    struct cord wall;
    printf("ID= %d Init calculate totalmomentum  \n",rank );
    //create wall up
    wall.x0=0;
    wall.y0=0;
    wall.x1=xsize;
    wall.y1=0;
    for (x=0; x<xsize; x++) {
        totalmomentum+=wall_collide(GetMatrixValue(&m,x,0),wall);
    }
    //create wall left
    wall.x0=0;
    wall.y0=0;
    wall.x1=0;
    wall.y1=ysize;
    for (y=0; y<ysize; y++) {
        totalmomentum+=wall_collide(GetMatrixValue(&m,0,y),wall);
    }
    //create wall right
    wall.x0=xsize;
    wall.y0=0;
    wall.x1=xsize;
    wall.y1=ysize;
    for (y=0; y<ysize; y++) {
        totalmomentum+=wall_collide(GetMatrixValue(&m,xsize-1,y),wall);
    }
    //create wall botton
    wall.x0=0;
    wall.y0=ysize;
    wall.x1=xsize;
    wall.y1=ysize;
    for (x=0; x<xsize; x++) {
        totalmomentum+=wall_collide(GetMatrixValue(&m,x,ysize-1),wall);
    }
    //printf("ID= %d finish calculate totalmomentum=%lf\n",rank,totalmomentum );

    printf("Total pressure =%lf\n", totalmomentum/(time_step*WALL_LENGTH));

    freeMatrix(&m);
    MPI_Finalize();
    return(0);
}
