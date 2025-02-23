
/*******************************************************************************
				MEMORY MAPPING THREADS
********************************************************************************
*
*	DESCRIPTION		memory_mapping_threads
*	AUTHOR 		Liad Raz
*	DATE			09 Feb 2022
*	CPYCOMPILE		gd src/memory_mapping_threads.c -pthread
*******************************************************************************/

#include <stdio.h>		/* printf, puts, stderr, NULL, fprintf */
#include <stdlib.h>		/* malloc, free*/
#include <pthread.h>		/* pthread_t, pthread_create, pthread_join 
					 pthread_exit */

#include <threads.h>		


#define AMOUNT_THREADS	4

long int global = 10000000000;

void MemoryMapping(void);
static void *thread1IMP(void *buffer_);
static void *thread2IMP(void *buffer_);


__thread int tlocal = 100;


int main(void)
{
	MemoryMapping();

	puts("USE IN GDB");

	return 0;
}


void MemoryMapping(void)
{
	pthread_t threads_id1 = {0};
	pthread_t threads_id2 = {0};

	if (0 != pthread_create(&threads_id1, NULL, thread1IMP, NULL))
	{
		fprintf(stderr, "threads_id has not created");
		return;
	}
	if (0 != pthread_create(&threads_id2, NULL, thread2IMP, NULL))
	{
		fprintf(stderr, "threads_id has not created");
		return;
	}


	if (0 != pthread_join(threads_id1, NULL))
	{
		fprintf(stderr, "threads_id has not joined");
		return;
	}
	if (0 != pthread_join(threads_id2, NULL))
	{
		fprintf(stderr, "threads_id has not joined");
		return;
	}
}


static void *thread1IMP(void *buffer_)
{
	int *mall = (int *)malloc(sizeof(100));
	size_t i = 0;
	
	while (i < 80000)
	{
		*mall = i * 20;
		++i;
	}
}


static void *thread2IMP(void *buffer_)
{
	int local = 100;
	tlocal = 200;
/* 	thread_local int tls_local = 10;
 */
	while (1)
	{
		tlocal *= 20;
	}
}
