#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <semaphore.h>

using namespace std;

#include "threadpool.h"

void test_fn(void *par)
{
	cout << "entering test_fn " << *(int *)par << endl;
	cout << "exiting test_fn " << *(int *)par << endl;
}

main ( )
{
	ThreadPool th(3);
	int max[20];

    	for (int i = 0; i < 5; i++) {
		    for (int j = 0; j < 4; j++) {
			    max[(i*4)+j] = 100 * ((i * 4) + j);
			    if (th.thread_avail())
			    {
			        th.dispatch_thread(test_fn,(void *)&(max[(i*4)+j]));
		        }
			    else
		       		cout << "No thread is avalable for "<< max[(i*4)+j] << endl;
		    }
		    sleep(6);        
    	} 
}


