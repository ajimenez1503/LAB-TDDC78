/*
File: blurfilter.c
Implementation of blurfilter function.
*/
#include "blurfilter.h"

void initDst(int xsize,int ysize){
    dst= (pixel *)calloc ( xsize*ysize,sizeof(pixel) );
}
void FreeDst(){
    free(dst);
}

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
void *blurfilter(void *data){
    struct thread_data *myData;
    myData = (struct thread_data *) data;
    /*
    int lsize=(myData->xsize*myData->ysize)/NUM_THREADS;
    int lysize=ysize/np;//number of row (of xsize pixel) per processors.
    int lsize=xsize*lysize;//number of pixel per processsors
    */
    int x=0,y=0,x2=0,y2=0, wi=0, i=0;
    double r=0.0,g=0.0,b=0.0,n=0.0, wc=0.0;
    //dst= (pixel *)calloc ( myData->xsize*myData->ysize,sizeof(pixel) );

    for (y=myData->ybegin; y<myData->yend; y++) {
        for (x=0; x<myData->xsize; x++) {
            r =  pix(myData->src_local, x, y, myData->xsize)->r;
            g =  pix(myData->src_local, x, y, myData->xsize)->g;
            b =  pix(myData->src_local, x, y, myData->xsize)->b;
            n = 1;
            for ( wi=1; wi <= myData->radius; wi++) {
                wc = myData->w[wi];
                x2 = x - wi;
                if(x2 >= 0) {
                    r += wc * pix(myData->src_local, x2, y, myData->xsize)->r;
                    g += wc * pix(myData->src_local, x2, y, myData->xsize)->g;
                    b += wc * pix(myData->src_local, x2, y, myData->xsize)->b;
                    n += wc;
                }
                x2 = x + wi;
                if(x2 < myData->xsize) {
                    r += wc * pix(myData->src_local, x2, y, myData->xsize)->r;
                    g += wc * pix(myData->src_local, x2, y, myData->xsize)->g;
                    b += wc * pix(myData->src_local, x2, y, myData->xsize)->b;
                    n += wc;
                }
            }
            pix(dst,x,y, myData->xsize)->r = r/n;
            pix(dst,x,y, myData->xsize)->g = g/n;
            pix(dst,x,y, myData->xsize)->b = b/n;
        }
    }
    pthread_mutex_lock( &mutex1 );
		number_threads_arrive++;
   	pthread_mutex_unlock( &mutex1 );
	//wait until all the thread write on tge dst
	while(number_threads_arrive<NUM_THREADS);
    //syncornization here

	//int aux_y=0;
    for (y=myData->ybegin; y<myData->yend; y++) {
        for (x=0; x<myData->xsize; x++) {
			r =  pix(dst, x, y, myData->xsize)->r;
			g =  pix(dst, x, y, myData->xsize)->g;
			b =  pix(dst, x, y, myData->xsize)->b;
			n = 1;
			for ( wi=1; wi <= myData->radius; wi++) {
				wc = myData->w[wi];
				//aux_y=y+lysize*rank;
				y2 = /*aux_y*/y - wi;

				if(y2 >= 0) {
				    r += wc * pix(dst, x, y2, myData->xsize)->r;
				    g += wc * pix(dst, x, y2, myData->xsize)->g;
				    b += wc * pix(dst, x, y2, myData->xsize)->b;
				    n += wc;
				}
				y2 = /*aux_y*/y + wi;
				if(y2 < myData->ysize) {
				    r += wc * pix(dst, x, y2, myData->xsize)->r;
				    g += wc * pix(dst, x, y2, myData->xsize)->g;
				    b += wc * pix(dst, x, y2, myData->xsize)->b;
				    n += wc;
				}
			}
			pix(myData->src_local,x,y, myData->xsize)->r = r/n;
			pix(myData->src_local,x,y, myData->xsize)->g = g/n;
			pix(myData->src_local,x,y, myData->xsize)->b = b/n;
	    }
	}
    pthread_exit(NULL);
	return NULL;
}
