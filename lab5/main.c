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
    //printf("ID= %d of number of processors %d\n",rank,np );

    //printf("ID= %d create topologies virtual n",rank);
    int dims[2]; // 2D matrix / grid
    dims[0]= 3; // 2 rows
    dims[1]= 3; // 2 columns
    int periods[2];
    periods[0]= 0; // Row-non-periodic
    periods[1]= 0; // Column-non-periodic
    int reorder = 0; // Re-order allowed
    MPI_Comm grid_comm;
    MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods,reorder,&grid_comm);

    /////////////Define type
    //printf("ID= %d define type particle \n",rank );
    MPI_Datatype PART_MPI; // MPI type to commit
    int block_lengths [] = { 1, 1, 1, 1 }; // Lengths of type elements
    MPI_Datatype block_types [] = { MPI_FLOAT,MPI_FLOAT,MPI_FLOAT,MPI_FLOAT }; //Set types
    MPI_Aint float_type, displacements[4];//number of elelment
    MPI_Type_extent(MPI_FLOAT, &float_type);//size of element char
    displacements[0] = 0;//where start the elemnt 0
    displacements[1] = float_type;//where start the elemnt 1
    displacements[2] = float_type+float_type;//where start the elemnt 2
    displacements[3] = float_type+float_type+float_type;//where start the elemnt 2
    MPI_Type_struct( 4, block_lengths, displacements, block_types, &PART_MPI );//createh the structure.
    MPI_Type_commit( &PART_MPI );


/*
    int coord[2]; // Cartesian Process coordinates
    MPI_Cart_coords(grid_comm,rank,2,coord);
    //printf("P:%d My coordinates are %d %d\n",rank,coord[0],coord[1]);
    int up,down,right,left;
    MPI_Cart_shift(grid_comm,0,1,&up,&down);
    MPI_Cart_shift(grid_comm,1,1,&left,&right);
    printf("P:%d My neighbors are r: %d d:%d 1:%d u:%d\n",rank,right,down,left,up);
*/
    //printf("ID= %d Init initialize \n",rank );
    srand(time(NULL) + rank); //use current time as seed for random generator
    int xsize=10, ysize=10;
    struct Matrix m = CreateNewMatrix(xsize,ysize);//x column and y row
    struct part_cord aux_part;
    float r=0, omega=0;
    int y=0,x=0;
    for (x=0; x<xsize; x++) {
        for (y=0; y<ysize; y++) {
            int coord[2]; // Cartesian Process coordinates
            MPI_Cart_coords(grid_comm,rank,2,coord);
            aux_part.y=y+coord[1]*ysize;//TODO take account of rank
            aux_part.x=x+coord[0]*xsize;//TODO take account of rank
            r=rand()*MAX_INITIAL_VELOCITY;
            omega=rand()*2*PI;
            aux_part.vx=r*cos(omega);
            aux_part.vy=r*sin(omega);
            SetMatrixValue(&m,x,y,aux_part);
            //printf("ID= %d  initialize (%d,%d) \n",rank,x,y );
        }
    }


/*
    printf("ID= %d prinf matrix \n",rank );
        for (x=0; x<xsize; x++) {
            for (y=0; y<ysize; y++) {
                printf("ID= %d. (%d,%d)  ",rank,(int)GetMatrixValue(&m,x,y)->x,(int)GetMatrixValue(&m,x,y)->y );
            }
            printf("\n");
        }
*/

    //printf("ID= %d Init check collision \n",rank );
    //create 4 buffer for every processors
    pcord_t * buffer_up = (pcord_t*)calloc(ysize, sizeof(pcord_t));
    pcord_t * buffer_down = (pcord_t*)calloc(ysize, sizeof(pcord_t));
    pcord_t * buffer_left = (pcord_t*)calloc(xsize, sizeof(pcord_t));
    pcord_t * buffer_right = (pcord_t*)calloc(xsize, sizeof(pcord_t));
    MPI_Status status;
    int time_step=1, max_time=3600;//1 min of time_step 60*60
    int up,down,right,left;
    MPI_Cart_shift(grid_comm,0,1,&up,&down);
    MPI_Cart_shift(grid_comm,1,1,&left,&right);
    //printf("P:%d My neighbors are r: %d d:%d 1:%d u:%d\n",rank,right,down,left,up);
    for(time_step=1; time_step<=max_time;time_step++){
        //check interation inside of the matrix. And calculate the buffer in the case of have neighbour
        for (x=0; x<xsize; x++) {
            for (y=0; y<ysize; y++) {
                if(x==0 && left!=-1){
                    buffer_left[y]=*GetMatrixValue(&m,x,y);
                }else if(x==xsize-1 && right!=-1){
                    buffer_right[y]=*GetMatrixValue(&m,x,y);
                }else if(y==0 && up!=-1){
                    buffer_up[x]=*GetMatrixValue(&m,x,y);
                }else if(y==ysize-1 && down!=-1){
                    buffer_down[x]=*GetMatrixValue(&m,x,y);
                }else if(x>0 && y>0 && x<xsize-1 && y<ysize-1){
                    if(collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x,y+1))==1 &&
                    collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x+1,y))==1    &&
                    collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x,y-1))==1 &&
                    collide(GetMatrixValue(&m,x,y), GetMatrixValue(&m,x-1,y))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,y),time_step) ;
                    }
                }
            }
        }
        //check interation the the right colum, without neighbors
        if(right==-1){
            for (x=0; x<xsize; x++) {
                if(x<xsize-1 && x>0){
                    if(collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x,ysize-2))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x+1,ysize-1))==1  &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x-1,ysize-1))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,ysize-1),time_step) ;
                    }
                }else if(x==xsize-1){
                    if(collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x,ysize-2))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x-1,ysize-1))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,ysize-1),time_step) ;
                    }
                }else if(x==0){
                    if(collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x,ysize-2))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x+1,ysize-1))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,ysize-1),time_step) ;
                    }
                }
            }
        }else{//if it has neighbour send buffer
            MPI_Send(buffer_right,xsize,PART_MPI,right,RIGHTTAG,grid_comm);
        }
        //check interation the the left colum, without neighbors
        if(left==-1){
            for (x=0; x<xsize; x++) {
                if(x<xsize-1 && x>0){
                    if(collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x,1))==1 &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x+1,0))==1  &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x-1,0))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,0),time_step) ;
                    }
                }else if(x==xsize-1){
                    if(collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x,1))==1 &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x-1,0))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,0),time_step) ;
                    }
                }else if(x==0){
                    if(collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x,1))==1 &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x+1,0))==1){ //dont move
                        feuler(GetMatrixValue(&m,x,0),time_step) ;
                    }
                }
            }
        }else{//if it has neighbour send buffer
            MPI_Send(buffer_left,xsize,PART_MPI,left,LEFTTAG,grid_comm);
        }
        //check interation the the up row, without neighbors
        if(up==-1){
            for (y=0; y<ysize; y++) {
                if(y<ysize-1 && y>0){
                    if(collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,1,y))==1 &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y+1))==1  &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y-1))==1){ //dont move
                        feuler(GetMatrixValue(&m,0,y),time_step) ;
                    }
                }else if(y==ysize-1){
                    if(collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,1,y))==1 &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y-1))==1 ){ //dont move
                        feuler(GetMatrixValue(&m,0,y),time_step) ;
                    }
                }else if(y==0){
                    if(collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,1,y))==1 &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y+1))==1 ){ //dont move
                        feuler(GetMatrixValue(&m,0,y),time_step) ;
                    }
                }
            }
        }else{//if it has neighbour send buffer
            MPI_Send(buffer_up,ysize,PART_MPI,up,UPTAG,grid_comm);
        }
        //check interation the the down row, without neighbors
        if(down==-1){
            for (y=0; y<ysize; y++) {
                if(y<ysize-1 && y>0){
                    if(collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-2,y))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y+1))==1  &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y-1))==1){ //dont move
                        feuler(GetMatrixValue(&m,xsize-1,y),time_step) ;
                    }
                }else if(y==ysize-1){
                    if(collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-2,y))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y-1))==1 ){ //dont move
                        feuler(GetMatrixValue(&m,xsize-1,y),time_step) ;
                    }
                }else if(y==0){
                    if(collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-2,y))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y+1))==1 ){ //dont move
                        feuler(GetMatrixValue(&m,xsize-1,y),time_step) ;
                    }
                }
            }
        }else{//if it has neighbour send buffer
            MPI_Send(buffer_down,ysize,PART_MPI,down,DOWNTAG,grid_comm);
        }
        //check interation the the left colum, with neighbors
        if(left!=-1){
            MPI_Recv(buffer_left,xsize,PART_MPI,left,RIGHTTAG,grid_comm,&status);
            for (x=0; x<xsize; x++) {
                if(x<xsize-1 && x>0){
                    if(collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x,1))==1 &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x+1,0))==1  &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x-1,0))==1 &&
                    collide(GetMatrixValue(&m,x,0), &buffer_left[x])==1){ //dont move
                        feuler(GetMatrixValue(&m,x,0),time_step) ;
                    }
                }else if(x==xsize-1){
                    if(collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x,1))==1 &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x-1,0))==1 &&
                    collide(GetMatrixValue(&m,x,0), &buffer_left[x])==1){ //dont move
                        feuler(GetMatrixValue(&m,x,0),time_step) ;
                    }
                }else if(x==0){
                    if(collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x,1))==1 &&
                    collide(GetMatrixValue(&m,x,0), GetMatrixValue(&m,x+1,0))==1 &&
                    collide(GetMatrixValue(&m,x,0), &buffer_left[x])==1){ //dont move
                        feuler(GetMatrixValue(&m,x,0),time_step) ;
                    }
                }
            }
        }
        //check interation the the right colum, with neighbors
        if(right!=-1){
            MPI_Recv(buffer_right,xsize,PART_MPI,right,LEFTTAG,grid_comm,&status);
            for (x=0; x<xsize; x++) {
                if(x<xsize-1 && x>0){
                    if(collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x,ysize-2))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x+1,ysize-1))==1  &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x-1,ysize-1))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), &buffer_right[x])==1){ //dont move
                        feuler(GetMatrixValue(&m,x,ysize-1),time_step) ;
                    }
                }else if(x==xsize-1){
                    if(collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x,ysize-2))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x-1,ysize-1))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), &buffer_right[x])==1){ //dont move
                        feuler(GetMatrixValue(&m,x,ysize-1),time_step) ;
                    }
                }else if(x==0){
                    if(collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x,ysize-2))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), GetMatrixValue(&m,x+1,ysize-1))==1 &&
                    collide(GetMatrixValue(&m,x,ysize-1), &buffer_right[x])==1){ //dont move
                        feuler(GetMatrixValue(&m,x,ysize-1),time_step) ;
                    }
                }
            }
        }
        //check interation the the down row, with neighbors
        if(down!=-1){
            MPI_Recv(buffer_down,ysize,PART_MPI,down,UPTAG,grid_comm,&status);
            for (y=0; y<ysize; y++) {
                if(y<ysize-1 && y>0){
                    if(collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-2,y))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y+1))==1  &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y-1))==1&&
                    collide(GetMatrixValue(&m,xsize-1,y), &buffer_down[y])==1){ //dont move
                        feuler(GetMatrixValue(&m,xsize-1,y),time_step) ;
                    }
                }else if(y==ysize-1){
                    if(collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-2,y))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y-1))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), &buffer_down[y])==1){ //dont move
                        feuler(GetMatrixValue(&m,xsize-1,y),time_step) ;
                    }
                }else if(y==0){
                    if(collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-2,y))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), GetMatrixValue(&m,xsize-1,y+1))==1 &&
                    collide(GetMatrixValue(&m,xsize-1,y), &buffer_down[y])==1){ //dont move
                        feuler(GetMatrixValue(&m,xsize-1,y),time_step) ;
                    }
                }
            }
        }
        //check interation the the up row, with neighbors
        if(up!=-1){
            MPI_Recv(buffer_up,ysize,PART_MPI,up,DOWNTAG,grid_comm,&status);
            for (y=0; y<ysize; y++) {
                if(y<ysize-1 && y>0){
                    if(collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,1,y))==1 &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y+1))==1  &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y-1))==1 &&
                    collide(GetMatrixValue(&m,0,y), &buffer_up[y])==1){ //dont move
                        feuler(GetMatrixValue(&m,0,y),time_step) ;
                    }
                }else if(y==ysize-1){
                    if(collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,1,y))==1 &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y-1))==1 &&
                    collide(GetMatrixValue(&m,0,y), &buffer_up[y])==1){ //dont move
                        feuler(GetMatrixValue(&m,0,y),time_step) ;
                    }
                }else if(y==0){
                    if(collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,1,y))==1 &&
                    collide(GetMatrixValue(&m,0,y), GetMatrixValue(&m,0,y+1))==1 &&
                    collide(GetMatrixValue(&m,0,y), &buffer_up[y])==1){ //dont move
                        feuler(GetMatrixValue(&m,0,y),time_step) ;
                    }
                }
            }
        }


    }
    printf("ID= %d finish check collision \n",rank );
    //TODO barrier
/*
    float momentum=0;
    //define wall
    struct cord wall;
    wall.x0=0;
    wall.y0=0;
    wall.x1=xsize-1;
    wall.y1=ysize-1;
    check with all the particles
    for (x=0; x<xsize; x++) {
        for (y=0; y<ysize; y++) {
            momentum+=wall_collide(GetMatrixValue(&m,x,y),wall);
        }
    }
    //printf("ID= %d finish calculate totalmomentum=%lf\n",rank,totalmomentum );
    MPIReduce( momentum, momentum, 1,MPI_FLOAT, SUM, 0,grid_comm );
    //reduction to id =0 and printf only id==0
    if(rank==0){
        printf("finish calculate totalmomentum=%lf\n",momentum );
        printf("Total pressure =%lf\n", momentum/(time_step*WALL_LENGTH));
    }
*/
    freeMatrix(&m);
    MPI_Finalize();
    return(0);
}
