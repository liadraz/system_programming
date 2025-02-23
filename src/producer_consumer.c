
/*******************************************************************************
					- PRODUCER_CONSUMER -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Implementation
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			07 Feb 2022
*
*******************************************************************************/

#include <stdio.h>		/* printf, puts, stderr, NULL, fprintf */
#include <stdlib.h>		/* calloc, malloc, free*/
#include <pthread.h>		/* pthread_t, pthread_create, pthread_join 
					 pthread_mutex_t, PTHREAD_MUTEX_INITIALIZER,
					 pthread_mutex_destroy, pthread_exit,
					 pthread_cond_t, pthread_cond_init, pthread_self,
					 pthread_cond_broadcast,  */
#include <semaphore.h>		/* sem_t,  SEM_FAILED, sem_open, sem_close, 
 						sem_unlink, sem_post, sem_wait */
#include <sys/stat.h>		/* O_RDWR */
#include <fcntl.h>		/* O_CREAT */
#include <stdatomic.h>		/* atomic_int, atomic_load, atomic_load */
#include <errno.h>		/* errno */
#include <assert.h>		/* assert */

#include "utilities.h"
#include "linked_list.h"	/* LListInsert, LListIsEmpty, LListRemove, 
						LListDestroy, LListGetData */
#include "producer_consumer.h"


/*******************************************************************************
**************************** Exerecise (1) ************************************/
#define ONE		1
#define THOUSAND	1000

typedef enum mtx {UNLOCKED, LOCKED} mtx_ty;

typedef struct buffer
{
	int *m_arr;
	int m_sum;
} buffer_ty;


/* volatile keyword that tells the COMPILER do no optimize it. I will change it later by a different thread */
/* atomic do not interrupt me when using me in the CPU.
	- In unix every variable is automatically calculated as atomic.
	- Using atomic is for portability manners.
	- Atomic operations must work with atomic variables.
 */
volatile atomic_int is_busy_g = 0;


static void *ProducerIMP(void *buffer);
static void *ConsumerIMP(void *buffer);


void ProducerConsumerExe1(void)
{
	pthread_t producer_id = 0;
	pthread_t consumer_id = 0;
	buffer_ty *buffer = NULL;

	buffer = (buffer_ty *)malloc(ONE * sizeof(buffer_ty));
	if (buffer == NULL)
	{
		return;
	}

	buffer->m_arr = (int *)calloc(THOUSAND , sizeof(int));
	if (buffer->m_arr == NULL)
	{
		free(buffer);
		return;
	}

	buffer->m_sum = 0;

	if (0 != pthread_create(&producer_id, NULL, ProducerIMP, buffer))
	{
		fprintf(stderr, "producer_id has not created");
		return;
	}

	if (0 != pthread_create(&consumer_id, NULL, ConsumerIMP, buffer))
	{
		fprintf(stderr, "consumer_id has not created");
		return;
	}


	if (0 != pthread_join(producer_id, NULL))
	{
		fprintf(stderr, "producer_id has not joined");
		return;
	}

	if (0 != pthread_join(consumer_id, NULL))
	{
		fprintf(stderr, "consumer_id has not joined");
		return;
	}

	puts("Exerecise (1)");
	printf("\tSum is:\t%d\n", buffer->m_sum);

	free(buffer->m_arr);
	free(buffer);
}

static void *ProducerIMP(void *buffer_)
{
	size_t i = 0;

	while (atomic_load(&is_busy_g))
	{
		;
	}
	
	atomic_store(&is_busy_g, LOCKED);

	for (; i < THOUSAND; ++i)
	{
		((buffer_ty *)buffer_)->m_arr[i] = 1;
		/*atomic_inc(((buffer_ty *)buffer_)->m_arr[i]);*/
	}

	atomic_store(&is_busy_g, UNLOCKED);

	return NULL;
}


static void *ConsumerIMP(void *buffer_)
{
	size_t i = 0;
	
	while (atomic_load(&is_busy_g))
	{
		;
	}

	atomic_store(&is_busy_g, LOCKED);

	for (; i < THOUSAND; ++i)
	{
		((buffer_ty *)buffer_)->m_sum += ((buffer_ty *)buffer_)->m_arr[i];
		/*atomic_add(((buffer_ty *)buffer_)->m_arr[i], ((buffer_ty *)buffer_)->m_sum);*/
	}

	atomic_store(&is_busy_g, UNLOCKED);

	return NULL;
}


/*******************************************************************************
**************************** Exerecise (2) ************************************/
#define NUM_OF_THREADS	8

/* Create Global Mutex */
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int global = 10;
static void *ProducerMutexIMP(void *list_);
static void *ConsumerMutexIMP(void *list_);


void ProducerConsumerExe2(void)
{
	pthread_t producer_id[NUM_OF_THREADS] = {0};
	pthread_t consumer_id[NUM_OF_THREADS] = {0};
	size_t i = 0;
	llist_ty *list = NULL;

	list = LListCreate();
	if (NULL == list)
	{
		puts("list: memory allocation failed");
		return;
	}

	for (i = 0; i < NUM_OF_THREADS; ++i)
	{
		if (0 != pthread_create(&producer_id[i], NULL, ProducerMutexIMP, list))
		{
			fprintf(stderr, "producer_id has not created");
			return;
		}

		if (0 != pthread_create(&consumer_id[i], NULL, ConsumerMutexIMP, list))
		{
			fprintf(stderr, "consumer_id has not created");
			return;
		}
	}

	for (i = 0; i < NUM_OF_THREADS; ++i)
	{
		if (0 != pthread_join(producer_id[i], NULL))
		{
			fprintf(stderr, "producer_id has not joined");
			return;
		}

		if (0 != pthread_join(consumer_id[i], NULL))
		{
			fprintf(stderr, "consumer_id has not joined");
			return;
		}
	}

	puts("Exerecise (2)");
	printf("\tSum of Nodes:\t%lu\n", LListCount(list));
	printf("\tList Is:\t%s\n", LListIsEmpty(list) ? "EMPTY" : "FULL");


	pthread_mutex_destroy(&mutex1);
	LListDestroy(list);
}


static void *ProducerMutexIMP(void *list_)
{
	assert (NULL != list_);
	
	/* Critical Section lock the Insertion */
	if (0 != pthread_mutex_lock(&mutex1))
	{
	
		return NULL;
	}
	LListInsert(LListEnd((llist_ty *)list_), NULL);
	if (0 != pthread_mutex_unlock(&mutex1))
	{
		return NULL;
	}

	pthread_exit(NULL);
}


static void *ConsumerMutexIMP(void *list_)
{
	/* In case list is empty :
		1. consumer thread is running before produce succeed to insert
		2. produce finished to insert all nodes. 
		consumer finished its job and need to break.
	*/

	assert (NULL != list_);

	/* lock section in order to use list functions */
	if (0 != pthread_mutex_lock(&mutex1))
	{
		return NULL;
	}

	/* In case list is empty, */
	while (LListIsEmpty(list_))
	{
		if (0 != pthread_mutex_unlock(&mutex1))
		{
			return NULL;
		}
		
		if (0 != pthread_mutex_lock(&mutex1))
		{
			return NULL;
		}
	}
	
	LListRemove(LListBegin((llist_ty *)list_));

	if (0 != pthread_mutex_unlock(&mutex1))
	{
		return NULL;
	}

	pthread_exit(NULL);
}

/*******************************************************************************
**************************** Exerecise (3) ************************************/
static void *ProducerMutexSemIMP(void *pkg_);
static void *ConsumerMutexSemIMP(void *pkg_);

/* Create Global Mutex */
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

typedef struct param
{
	llist_ty *m_list;
	sem_t *m_semaphore;
} param_ty;


void ProducerConsumerExe3(void)
{
	pthread_t producer_id[NUM_OF_THREADS] = {0};
	pthread_t consumer_id[NUM_OF_THREADS] = {0};
	param_ty pkg = {NULL};
	sem_t *semaphore = NULL;
	llist_ty *list = NULL;
	char *sem_name = "sem";
	size_t i = 0;

	list = LListCreate();
	if (NULL == list)
	{
		puts("list: memory allocation failed");
		return;
	}

	semaphore = sem_open(sem_name, O_CREAT | O_EXCL, O_RDWR, 0);
	if (semaphore == SEM_FAILED)
	{
		LListDestroy(list);
		puts("semaphore: allocation failed");
		printf("sem_open: Value of errno %d\n", errno);
		return;
	}

	/* Init pkg members */
	pkg.m_list = list;
	pkg.m_semaphore = semaphore;

	for (i = 0; i < NUM_OF_THREADS; ++i)
	{
		if (0 != pthread_create(&producer_id[i], NULL, ProducerMutexSemIMP, &pkg))
		{
			fprintf(stderr, "producer_id has not created");
			return;
		}

		if (0 != pthread_create(&consumer_id[i], NULL, ConsumerMutexSemIMP, &pkg))
		{
			fprintf(stderr, "consumer_id has not created");
			return;
		}
	}

	for (i = 0; i < NUM_OF_THREADS; ++i)
	{
		if (0 != pthread_join(producer_id[i], NULL))
		{
			fprintf(stderr, "producer_id has not joined");
			return;
		}

		if (0 != pthread_join(consumer_id[i], NULL))
		{
			fprintf(stderr, "consumer_id has not joined");
			return;
		}
	}

	puts("Exerecise (3)");
	printf("\tSum of Nodes:\t%lu\n", LListCount(list));
	printf("\tList Is:\t%s\n", LListIsEmpty(list) ? "EMPTY" : "FULL");

	/* free semaphore, mutex and list */
	if (0 != sem_close(semaphore))
	{
		puts("sem_close: Error Occurred");
		printf("sem_close: Value of errno %d\n", errno);
		return;
	}

	if (0 != sem_unlink(sem_name))
	{
		puts("sem_unlink: Error Occurred");
		printf("sem_unlink: Value of errno %d\n", errno);
		return;
	}
	
	if (0 != pthread_mutex_destroy(&mutex1))
	{
		puts("pthread_mutex_destroy: Error Occurred");
	}

	LListDestroy(list);
}

static void *ProducerMutexSemIMP(void *pkg_)
{
	llist_itr_ty itr_ret = {NULL};

	assert (NULL != ((param_ty *)pkg_)->m_list);

	/* Critical Section lock the Insertion */
	pthread_mutex_lock(&mutex2);
	itr_ret = LListInsert(LListEnd(((param_ty *)pkg_)->m_list), NULL);
	pthread_mutex_unlock(&mutex2);

	/* In case insertion did not work, do not increment the semaphore */
	if (LListIsSameIter(itr_ret, LListEnd(((param_ty *)pkg_)->m_list)))
	{
		fprintf(stderr, "sem_post failed");
		return NULL;
	}

	if (0 != sem_post(((param_ty *)pkg_)->m_semaphore))
	{
		fprintf(stderr, "sem_post failed");
	}

	pthread_exit(NULL);
}


static void *ConsumerMutexSemIMP(void *pkg_)
{
	assert (NULL != ((param_ty *)pkg_)->m_list);

	/* In case semaphore is zero, block the thread */
	if (0 != sem_wait(((param_ty *)pkg_)->m_semaphore))
	{
		fprintf(stderr, "sem_wait failed");
		return NULL;
	}

	/* Critical Section lock the Removal */
	pthread_mutex_lock(&mutex2);
	LListRemove(LListBegin(((param_ty *)pkg_)->m_list));
	pthread_mutex_unlock(&mutex2);
	
	
	pthread_exit(NULL);
}

/*******************************************************************************
**************************** Exerecise (6) ************************************/
static void *ProducerMutexSemIMP(void *pkg_);
static void *ConsumerMutexSemIMP(void *pkg_);

#define CONSUMERS	8

/* Create Global Mutex */
pthread_mutex_t mutex6 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = {0};

/* While using cond_wait a thread might get woken up even though no thread signalled the condition. wait_status will ensure that only when broadcast provided the threads will
be released from the condition */
int wait_status = 0;

void *ProducerMP(void *pkg_);
void *ConsumersIMP(void *pkg_);


void ProducerConsumerExe6(void)
{
	pthread_t producer_id = {0};
	pthread_t consumer_id[CONSUMERS] = {0};
	param_ty pkg = {NULL};
	sem_t *semaphore = NULL;
	llist_ty *list = NULL;
	char *sem_name = "sem";
	size_t i = 0;

	list = LListCreate();
	if (NULL == list)
	{
		puts("list: memory allocation failed");
		return;
	}

	semaphore = sem_open(sem_name, O_CREAT | O_EXCL, O_RDWR, 0);
	if (semaphore == SEM_FAILED)
	{
		LListDestroy(list);
		puts("semaphore: allocation failed");
		printf("sem_open: Value of errno %d\n", errno);
		return;
	}

	if (0 != pthread_cond_init(&cond_var, NULL))
	{
		fprintf(stderr, "cond_var has not created");
		return;
	}

	/* Init pkg members */
	pkg.m_list = list;
	pkg.m_semaphore = semaphore;

	/* Create only one producer which will insert one node to list */
	if (0 != pthread_create(&producer_id, NULL, ProducerMP, &pkg))
	{
		fprintf(stderr, "producer_id has not created");
		return;
	}

	for (i = 0; i < CONSUMERS; ++i)
	{

		if (0 != pthread_create(&consumer_id[i], NULL, ConsumersIMP, &pkg))
		{
			fprintf(stderr, "consumer_id has not created");
			return;
		}
	}


	if (0 != pthread_join(producer_id, NULL))
	{
		fprintf(stderr, "producer_id has not joined");
		return;
	}

	for (i = 0; i < CONSUMERS; ++i)
	{

		if (0 != pthread_join(consumer_id[i], NULL))
		{
			fprintf(stderr, "consumer_id has not joined");
			return;
		}
	}


	/* free semaphore, mutex and list */
	if (0 != sem_close(semaphore))
	{
		puts("sem_close: Error Occurred");
		printf("sem_close: Value of errno %d\n", errno);
		return;
	}

	if (0 != sem_unlink(sem_name))
	{
		puts("sem_unlink: Error Occurred");
		printf("sem_unlink: Value of errno %d\n", errno);
		return;
	}
	
	if (0 != pthread_mutex_destroy(&mutex1))
	{
		puts("pthread_mutex_destroy: Error Occurred");
	}

	LListDestroy(list);
}


void *ProducerMP(void *pkg_)
{
	size_t i = 0;
	llist_itr_ty itr_ret = {NULL};
	pthread_t tid = 0;

	assert (NULL != ((param_ty *)pkg_)->m_list);

	/* Insert only one node to the list */
	pthread_mutex_lock(&mutex6);
	tid = pthread_self();
	itr_ret = LListInsert(LListEnd(((param_ty *)pkg_)->m_list), &tid);
	pthread_mutex_unlock(&mutex6);

	/* In case insertion did not work, do not increment the semaphore */
	if (LListIsSameIter(itr_ret, LListEnd(((param_ty *)pkg_)->m_list)))
	{
		fprintf(stderr, "sem_post failed");
		return NULL;
	}


	/* Fill the semaphores with the provided consumers */
	for (; i < CONSUMERS; ++i)
	{
		if (0 != sem_wait(((param_ty *)pkg_)->m_semaphore))
		{
			fprintf(stderr, "sem_wait failed");
			return NULL;
		}
	}

	pthread_mutex_lock(&mutex6);
	wait_status = 0;
	pthread_cond_broadcast(&cond_var);
	wait_status = 1;
	pthread_mutex_unlock(&mutex6);


	/* Empty the semaphore which will indicate that job is done  */
	for (; i < CONSUMERS; ++i)
	{
		if (0 != sem_wait(((param_ty *)pkg_)->m_semaphore))
		{
			fprintf(stderr, "sem_wait failed");
			return NULL;
		}
	}

	pthread_exit(NULL);
}


void *ConsumersIMP(void *pkg_)
{
	void *data = NULL;

	if (0 != sem_post(((param_ty *)pkg_)->m_semaphore))
	{
		fprintf(stderr, "sem_post failed");
	}

	pthread_mutex_lock(&mutex6);

	/* wait_status protects the cond_wait function in case of spurious wakeup */
	while (!wait_status)
	{
		pthread_cond_wait(&cond_var, &mutex6);
	}
	data = LListGetData(LListBegin(((param_ty *)pkg_)->m_list));
	printf("data %lu\n", *(long int *)data);
	pthread_mutex_unlock(&mutex6);

	/* Check again if all consumers read the list's node */
	if (0 != sem_post(((param_ty *)pkg_)->m_semaphore))
	{
		fprintf(stderr, "sem_post failed");
	}

	pthread_exit(NULL);
}