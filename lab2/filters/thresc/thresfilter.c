#include "thresfilter.h"

#define uint unsigned int

void * thresfilter(void *data)
{
	struct thread_data *myData;
	myData = (struct thread_data *) data;
	int i=0;
    for(i = myData->begin; i <myData->end; i++) {
        myData->sum += (uint)myData->src_local[i].r + (uint)myData->src_local[i].g + (uint)myData->src_local[i].b;
    }
	pthread_mutex_lock( &mutex1 );
	   	SumTotal+=myData->sum;
		number_threads_arrive++;
	   	//printf("SumTotal value: %d\n",SumTotal);
   	pthread_mutex_unlock( &mutex1 );
	//wait until all the thread write on the SumTotal
	while(number_threads_arrive<NUM_THREADS);

	pthread_mutex_lock( &mutex1 );
	   	myData->sum=SumTotal/((myData->end-myData->begin)*NUM_THREADS);
		//printf("SumTotal average: %d\n",myData->sum);
   	pthread_mutex_unlock( &mutex1 );
	int psum=0;
	for(i = myData->begin; i <myData->end; i++) {
        psum = (uint)myData->src_local[i].r + (uint)myData->src_local[i].g + (uint)myData->src_local[i].b;
        if(myData->sum > psum) {
            myData->src_local[i].r = myData->src_local[i].g = myData->src_local[i].b = 0;
        }
        else {
            myData->src_local[i].r = myData->src_local[i].g = myData->src_local[i].b = 255;
        }
    }
   	pthread_exit(NULL);
	return NULL;
}
