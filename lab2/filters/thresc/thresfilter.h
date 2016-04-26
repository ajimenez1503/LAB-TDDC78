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
#define uint unsigned int

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

typedef struct thread_data{
	int sum;
    int begin;
    int end;
}thread_data ;

static pixel* src_local;
static int SumTotal=0;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;//mutex for calculate on sumtotla
static int number_threads_arrive=0;//check that all the thread write in SumTotal

void initMemory(pixel* src);//init shared memory
void *thresfilter(void *data);

#endif
