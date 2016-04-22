/*
File: blurfilter.c
Implementation of blurfilter function.
*/
#include "blurfilter.h"

pixel* pix(pixel* image,int xx,int yy, const int xsize)
{
    register int off = xsize*yy + xx;
    #ifdef DBG
    if(off >= MAX_PIXELS) {
        fprintf(stderr, "n Terribly wrong: %d %d %dn",xx,yy,xsize);
    }
    #endif
    return (image + off);
}
void blurfilter(int xsize,int ysize, pixel* src, int radius, double *w){
	

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

    int np=0,rank=0;
    MPI_Comm_size (MPI_COMM_WORLD, &np);        // get number of processes
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        // get current process id
    MPI_Comm com = MPI_COMM_WORLD;
    MPI_Bcast( &ysize, 1, MPI_INT, 0, com );
    MPI_Bcast( &xsize, 1, MPI_INT, 0, com );
    MPI_Bcast( &radius, 1, MPI_INT, 0, com );
    MPI_Bcast( w, radius+1, MPI_DOUBLE, 0, com );//all need to read the weight
    int lysize=ysize/np;//number of row (of xsize pixel) per processors.
    int lsize=xsize*lysize;//number of pixel per processsors
	
    int x=0,y=0,x2=0,y2=0, wi=0;
    double r=0.0,g=0.0,b=0.0,n=0.0, wc=0.0;
    //pixel * src_local= calloc ( lsize,sizeof(pixel) );
    //pixel * dst_local= calloc ( lsize,sizeof(pixel) );
    pixel src_local[lsize];// local buffer
    pixel dst_local[lsize];// local buffer
	//printf("id %d of %d\n",rank,np);
    pixel * dst= calloc ( xsize*ysize,sizeof(pixel) );
	

    MPI_Scatter( src, lsize, PIXEL_MPI,src_local,lsize, PIXEL_MPI, 0,com);//divide the src between the processsors in src_local

    for (y=0; y<lysize; y++) {
        for (x=0; x<xsize; x++) {
            r =  pix(src_local, x, y, xsize)->r;
            g =  pix(src_local, x, y, xsize)->g;
            b =  pix(src_local, x, y, xsize)->b;
            n = 1;
            for ( wi=1; wi <= radius; wi++) {
                wc = w[wi];
                x2 = x - wi;
                if(x2 >= 0) {
                    r += wc * pix(src_local, x2, y, xsize)->r;
                    g += wc * pix(src_local, x2, y, xsize)->g;
                    b += wc * pix(src_local, x2, y, xsize)->b;
                    n += wc;
                }
                x2 = x + wi;
                if(x2 < xsize) {
                    r += wc * pix(src_local, x2, y, xsize)->r;
                    g += wc * pix(src_local, x2, y, xsize)->g;
                    b += wc * pix(src_local, x2, y, xsize)->b;
                    n += wc;
                }
            }
            pix(dst_local,x,y, xsize)->r = r/n;
            pix(dst_local,x,y, xsize)->g = g/n;
            pix(dst_local,x,y, xsize)->b = b/n;
        }
    }
    MPI_Allgather( dst_local, lsize, PIXEL_MPI,dst, lsize, PIXEL_MPI,com );
	int aux_y=0;
	for (y=0; y<lysize; y++) {
	    for (x=0; x<xsize; x++) {
			r =  pix(dst_local, x, y, xsize)->r;
			g =  pix(dst_local, x, y, xsize)->g;
			b =  pix(dst_local, x, y, xsize)->b;
			n = 1;
			for ( wi=1; wi <= radius; wi++) {
				wc = w[wi];
				aux_y=y+lysize*rank;
				y2 = aux_y - wi;
			
				if(y2 >= 0) {
				    r += wc * pix(dst, x, y2, xsize)->r;
				    g += wc * pix(dst, x, y2, xsize)->g;
				    b += wc * pix(dst, x, y2, xsize)->b;
				    n += wc;
				}
				y2 = aux_y + wi;
				if(y2 < ysize) {
				    r += wc * pix(dst, x, y2, xsize)->r;
				    g += wc * pix(dst, x, y2, xsize)->g;
				    b += wc * pix(dst, x, y2, xsize)->b;
				    n += wc;
				}
			}
			pix(src_local,x,y, xsize)->r = r/n;
			pix(src_local,x,y, xsize)->g = g/n;
			pix(src_local,x,y, xsize)->b = b/n;
	    }
	}

    MPI_Gather( src_local, lsize, PIXEL_MPI,src, lsize, PIXEL_MPI, 0, com );
    free (dst);
    MPI_Type_free(&PIXEL_MPI);
    //free(dst_local);
    //free(src_local);
}
