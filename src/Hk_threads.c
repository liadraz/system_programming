
/*******************************************************************************
					- HK_THREADS -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		100K Threads Exerecise
*	AUTHOR 		Liad Raz
*	REVIEWER		Omri
*	DATE			02 Feb 2022
*	CPYCOMPILE		gc -pthread src/Hk_threads.c
*
*******************************************************************************/

#include <stdio.h>			/* fputs, putchar, stderr */
#include <stdlib.h>			/* calloc, free */
#include <unistd.h>			/* sleep */
#include <time.h>				/* time, time_t */
#include <sys/time.h>			/* timeval */
#include <pthread.h>			/* pthread_t, pthread_create */
#include <omp.h>				/* omp_get_thread_num */

#define ONE_THOUSAND	100000
#define MAX_U_THREADS	32000		/* Max threads per process is 32751 */


typedef struct arr_ints
{
	int m_threads[ONE_THOUSAND];
	size_t m_size;
} arr_ints_ty;

/* Init global array struct, each thread will get a specified index*/
arr_ints_ty ints_g = {{0}, ONE_THOUSAND};


typedef struct sum_divisor
{
	size_t m_end;
	size_t m_start;
	size_t m_num;
	size_t m_sum_divisors;
	size_t m_num_divisors;
} sum_divisor_ty;


static void *IncrementIndex(void *index_);
static void *SumOfDivisors(void *param_);

/*******************************************************************************
**************************** Exerecise (1) ************************************/
void Create100KThreadsExe1(void)
{
	pthread_t thread_id = 0;
	size_t i = 0;
	time_t begin = 0;
	time_t end = 0;

	int ret_status = 0;
	size_t not_created_ctr = 0;

	begin = time(NULL);
	/* Create 100K threads */
	for (i = 0; i < ONE_THOUSAND; ++i)
	{
		ret_status = pthread_create(&thread_id, NULL, IncrementIndex, &i);

		/* Count the created Threads */
		if (0 == ret_status)
		{
			++not_created_ctr;
		}
	}
	end = time(NULL);

	sleep(10);

	/* Print The Array of ints */
	for (i = 0; i < ONE_THOUSAND; ++i)
	{
		printf("%lu.(%d)\t", i, ints_g.m_threads[i]);
	}

	printf("\n\nRunning Time Elapsed \t%lu", end - begin);
	printf("\nThreads Not Created \t%lu", not_created_ctr); /* ==> 32751 */
}

static void *IncrementIndex(void *index_)
{
	size_t index = *(size_t *)index_;

	ints_g.m_threads[index] = index;

	return NULL;
}


/*******************************************************************************
**************************** Exerecise (2) ************************************/
void Create100KThreadsExe2(void)
{
	pthread_t thread_id = 0;
	size_t i = 0;
	time_t begin = 0;
	time_t end = 0;
	int ret_status = 0;
	size_t not_created_ctr = 0;
	size_t not_index_ctr = 0;

	begin = time(NULL);
	/* Create 100K threads */
	for (i = 0; i < ONE_THOUSAND; ++i)
	{
		ret_status = pthread_create(&thread_id, NULL, IncrementIndex, &i);

		if (0 != ret_status)
		{
			++not_created_ctr;
		}
		
		while (0 != ret_status)
		{
			ret_status = pthread_create(&thread_id, NULL, IncrementIndex, &i);
		}
	}
	end = time(NULL);
	
	sleep(10);

	/* Print The Array of ints */
	for (i = 0; i < ONE_THOUSAND; ++i)
	{
		if (i == (size_t)ints_g.m_threads[i])
		{
			++not_index_ctr;
		}
		
		printf("%lu.(%d)\t", i, ints_g.m_threads[i]);
	}

	printf("\n\nRunning Time Elapsed \t%lu", end - begin);
	printf("\nNot Created Counter =>\t%lu", not_created_ctr);
	printf("\nNot Indexed Counter =>\t%lu", not_index_ctr);
}


/*******************************************************************************
**************************** Exerecise (3) ************************************/
void Create100KThreadsExe3(void)
{
	pthread_t thread_id = 0;
	size_t i = 0;
	time_t begin = 0;
	time_t end = 0;
	int ret_status = 0;
	size_t not_created_ctr = 0;
	size_t not_index_ctr = 0;

	begin = time(NULL);
	/* Create 100K threads */
	for (i = 0; i < ONE_THOUSAND; ++i)
	{
		ret_status = pthread_create(&thread_id, NULL, IncrementIndex, &i);
		pthread_detach(thread_id);

		if (0 != ret_status)
		{
			++not_created_ctr;
		}

		while (0 != ret_status)
		{
			ret_status = pthread_create(&thread_id, NULL, IncrementIndex, &i);
		}
	}
	end = time(NULL);
	
	sleep(10);

	/* Print The Array of ints */
	for (i = 0; i < ONE_THOUSAND; ++i)
	{
		if (i == (size_t)ints_g.m_threads[i])
		{
			++not_index_ctr;
		}

		printf("%lu.(%d)\t", i, ints_g.m_threads[i]);
	}

	printf("\n\nRunning Time Elapsed \t%lu", end - begin);
	printf("\nNot Created Counter =>\t%lu", not_created_ctr);
	printf("\nNot Indexed Counter =>\t%lu", not_index_ctr);
}


/*******************************************************************************
**************************** Exerecise (4) ************************************/
void Create100KThreadsExe4(size_t num_, size_t threads_)
{
	sum_divisor_ty *param = NULL;
	pthread_t *thread_id = NULL;
	size_t range = 0;
	size_t total_sum = 0;
	size_t total_num = 0;
	size_t index = 0;
	int ret_status = 0;
	time_t begin = 0;
	time_t end = 0;

	param = (sum_divisor_ty *)calloc(threads_, sizeof(sum_divisor_ty));
	thread_id = (pthread_t *)calloc(threads_, sizeof(pthread_t));

	range = num_ / threads_;

	begin = time(NULL);
	/* Create the Maximum amount of threads linux can handle */
	for (index = 0; index < threads_ - 1; ++index)
	{
		param[index].m_end = (index + 1) * range;
		param[index].m_start = param[index].m_end - range + 1;
		param[index].m_num = num_;

		ret_status = pthread_create(&thread_id[index], NULL, SumOfDivisors, &param[index]);
	}
	end = time(NULL);

	/* Calculate the last thread in case there is a remainder */
	param[index].m_end = num_;
	param[index].m_start = param[index].m_end - range + 1;
	param[index].m_num = num_;

	ret_status = pthread_create(&thread_id[index], NULL, SumOfDivisors, &param[index]);


	/* Count the sum of divisors */
	for (index = 0; index < threads_; ++index)
	{
		pthread_join(thread_id[index], NULL);

		total_sum += param[index].m_sum_divisors;
		total_num += param[index].m_num_divisors;
	}

	printf("\n\nNum Of Threads \t%lu", threads_);
	printf("\n\nRunning Time Elapsed \t%lu", end - begin);
	printf("\nTotal Sum Divisors\t%lu", total_sum);
	printf("\nTotal Num Divisors\t%lu\n\n", total_num);

	free(param);
	free(thread_id);
}


static void *SumOfDivisors(void *param_)
{
	size_t i = ((sum_divisor_ty *)param_)->m_start;
	for (; i <= ((sum_divisor_ty *)param_)->m_end; ++i)
	{
		if (0 == ((sum_divisor_ty *)param_)->m_num % i)
		{
			((sum_divisor_ty *)param_)->m_sum_divisors += i;
			((sum_divisor_ty *)param_)->m_num_divisors += 1;
		}
	}

	return param_;
}

/*
	Threads	Time ms
	3		0m3.509s
	4		0m2.633s
	5		0m2.259s
	6		0m1.893s
	7		0m1.635s	
	8		0m1.431s
	9		0m1.625s
	10		0m1.576s
	11		0m1.500s
	12		0m1.614s
	13		0m1.515s
	14		0m1.507s
	15		0m1.478s
	100		0m1.442s

	One Loop
	Threads	Time ms
	1		0m10.136s
	fopenmp	0m10.112s

*/

/*******************************************************************************
**************************** Exerecise (5) ************************************/
void Create100KThreadsExe5(size_t num_)
{
	size_t i = 0;
	size_t start = 1;
	size_t end = num_;
	size_t sum_divisors = 0;
	size_t num_divisors = 0;
	
	time_t begint = 0;
	time_t endt = 0;

	begint = time(NULL);
	/* In order to compile openMP use this sentence =>
	Cpy Compile gc src/Hk_threads.c test/Hk_threads_test.c -I ../ -I include/ -fopenmp -o Hk.out 
	*/
	#pragma omp parallel for
	for (i = start; i <= end; ++i)
	{
            /*printf("%lu\t%d\n", i, omp_get_thread_num());*/
		if (0 == num_ % i)
		{
			sum_divisors += i;
			++num_divisors;
		}

	}
	endt = time(NULL);

	printf("\n\nRunning Time Elapsed \t%lu", endt - begint);
	puts("\n\nNum Of Threads 1\t");
	printf("\nTotal Sum Divisors\t%lu", sum_divisors);
	printf("\nTotal Num Divisors\t%lu", num_divisors);
}