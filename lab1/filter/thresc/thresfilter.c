#include "thresfilter.h"

#define uint unsigned int

void thresfilter(int size,pixel* src_total){
    /////////////Define type
    MPI_Datatype PIXEL_MPI; // MPI type to commit
    int block_lengths [] = { 1, 1, 1 }; // Lengths of type elements
    MPI_Datatype block_types [] = { MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR,MPI_UNSIGNED_CHAR }; //Set types
    MPI_Aint char_type, displacements[3];//number of elelment
    MPI_Type_extent(MPI_UNSIGNED_CHAR, &char_type);//size of element char
    displacements[0] = 0;//where start the elemnt 0
    displacements[1] = char_type;//where start the elemnt 1
    displacements[2] = char_type+char_type;//where start the elemnt 2
    MPI_Type_struct( 3, block_lengths, displacements, block_types, &PIXEL_MPI );//createh the structure.
    MPI_Type_commit( &PIXEL_MPI );
    /////////////Define type

    int np=0,sumLocal=0,sumTotal=0, i=0, psum=0;
    MPI_Comm_size (MPI_COMM_WORLD, &np);        /* get number of processes */
    MPI_Comm com = MPI_COMM_WORLD;
    MPI_Bcast( &size, 1, MPI_INT, 0, com );
    int lsize = size / np; // local problem size
    pixel src_local[lsize];//crateh the local buffer
    MPI_Scatter( src_total, lsize, PIXEL_MPI,src_local,lsize, PIXEL_MPI, 0,com);//divide the src between the processsors


    //calculat the sum in every processors
    for(i = 0; i <lsize; i++) {
        sumLocal += (uint)src_local[i].r + (uint)src_local[i].g + (uint)src_local[i].b;
    }

    //calcualte average all all the sum and divide total
    MPI_Allreduce( &sumLocal, &sumTotal, 1, MPI_INT,MPI_SUM, com );
    sumTotal /= size;

    //check if it is black or white in every buffer
    for(i = 0; i <lsize; i++) {
        psum = (uint)src_local[i].r + (uint)src_local[i].g + (uint)src_local[i].b;
        if(sumTotal > psum) {
            src_local[i].r = src_local[i].g = src_local[i].b = 0;
        }
        else {
            src_local[i].r = src_local[i].g = src_local[i].b = 255;
        }
    }

    //gather the buffer of every processors  in id==0
    MPI_Gather( &src_local, lsize, PIXEL_MPI,src_total, lsize, PIXEL_MPI, 0, com );
    MPI_Type_free(&PIXEL_MPI);
}
