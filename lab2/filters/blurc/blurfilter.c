/*
File: blurfilter.c
Implementation of blurfilter function.
*/
#include "blurfilter.h"
//init shared memory
void initMemory(int x,int y,int r, pixel* src,double *weight){
    src_local=src;
    dst= (pixel *)calloc ( x*y,sizeof(pixel) );
    xsize=x;
    ysize=y;
    radius=r;
    w=weight;
}
void FreeDst(){
    free(dst);
}

pixel* pix(pixel* image,int xx,int yy, const int xSize)
{
    register int off = xSize*yy + xx;
    #ifdef DBG
    if(off >= MAX_PIXELS) {
        fprintf(stderr, "n Terribly wrong: %d %d %dn",xx,yy,xSize);
    }
    #endif
    return (image + off);
}
void *blurfilter(void *data){
    struct thread_data *myData;
    myData = (struct thread_data *) data;

    int x=0,y=0,x2=0,y2=0, wi=0, i=0;
    double r=0.0,g=0.0,b=0.0,n=0.0, wc=0.0;


    for (y=myData->ybegin; y<myData->yend; y++) {
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
            pix(dst,x,y, xsize)->r = r/n;
            pix(dst,x,y, xsize)->g = g/n;
            pix(dst,x,y, xsize)->b = b/n;
        }
    }
    pthread_mutex_lock( &mutex1 );
		number_threads_arrived++;
   	pthread_mutex_unlock( &mutex1 );
	//wait until all the thread write on tge dst
	while(number_threads_arrived<NUM_THREADS);
    //syncornization here

    for (y=myData->ybegin; y<myData->yend; y++) {
        for (x=0; x<xsize; x++) {
			r =  pix(dst, x, y, xsize)->r;
			g =  pix(dst, x, y, xsize)->g;
			b =  pix(dst, x, y, xsize)->b;
			n = 1;
			for ( wi=1; wi <= radius; wi++) {
				wc = w[wi];
				y2 = y - wi;
				if(y2 >= 0) {
				    r += wc * pix(dst, x, y2, xsize)->r;
				    g += wc * pix(dst, x, y2, xsize)->g;
				    b += wc * pix(dst, x, y2, xsize)->b;
				    n += wc;
				}
				y2 =y + wi;
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
    pthread_exit(NULL);
	return NULL;
}
