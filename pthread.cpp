#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREAD 4

typedef struct param
{
	int in;
	int out;
}param;

void* thread(void * p)
{
	param *para = (param *)p;
	printf("I am a thread %d\n", para->in);
	sleep(para->in);
	para->out = para->in + 100;
	pthread_exit(&para->out);
	return NULL;
}

int main()
{
	int i;
	param paras[NUM_THREAD];
	pthread_t threads[NUM_THREAD];
	// create pthreads
	for(i = 0; i < NUM_THREAD; i++)
	{
		paras[i].in = i;
		pthread_create(threads+i, NULL, thread, paras+i);
	}
	// wait pthreads exit
	int* ret[NUM_THREAD];
	for(i = 0; i < NUM_THREAD; i++)
	{
		pthread_join(threads[i], (void**)(ret+i));
		printf("%d exit %d = %d\n", i, *ret[i], paras[i].out);
	}
	return 0;
}

