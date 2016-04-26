#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
/*
  File: thresfilter.h

  Declaration of pixel structure and thresfilter function.

 */
#ifndef _THRESFILTER_H_
#define _THRESFILTER_H_
#define NUM_THREADS	5


/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

typedef struct thread_data{
	pixel* src_local;
	int sum;
    int begin;
    int end;
}thread_data ;


static int SumTotal=0;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;//mutex for calculate on sumtotla
static int number_threads_arrive=0;//check that all the thread write in SumTotal


void *thresfilter(void *data);

#endif
