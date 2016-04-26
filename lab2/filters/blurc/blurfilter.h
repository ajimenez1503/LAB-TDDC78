/*
  File: blurfilter.h
  Declaration of pixel structure and blurfilter function.
 */
#ifndef _BLURFILTER_H_
#define _BLURFILTER_H_
#include <pthread.h>
#include <stdio.h>
#include "ppmio.h"
#include <stdlib.h>

#define NUM_THREADS	5

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

typedef struct thread_data{
	pixel* src_local;
    int ybegin;
    int yend;
    int xsize;
    int ysize;
    double *w;
    int radius;
}thread_data ;

static  pixel *  dst;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;//mutex for calculate on sumtotla
static int number_threads_arrive=0;//check that all the thread write in SumTotal

void initDst(int xsize,int ysize);
void FreeDst();
void *blurfilter(void *data);

#endif
