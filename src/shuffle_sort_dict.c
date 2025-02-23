
/*******************************************************************************
					- SHUFFLE_SORT_DICT -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Implementation
*	AUTHOR 		Liad Raz
*	REVIEWER		David
*	DATE			20 Feb 2022
*
*******************************************************************************/

#include <stdio.h>	/* EOF */
#include <assert.h>	/* assert */
#include <stdlib.h>	/* malloc, free, qsort, rand, srand */
#include <sys/stat.h>	/* stat, fstat */
#include <fcntl.h>	/* open */
#include <sys/mman.h>	/* mmap, munmap */
#include <unistd.h>	/* close */
#include <string.h>	/* memmove, strcmp */
#include <time.h>		/* time */
#include <pthread.h>	/* pthread_create, pthread_exit */


#include "utilities.h"
#include "shuffle_sort_dict.h"

#define DICT_FILE_PATH		"/usr/share/dict/american-english"
#define NUM_OF_CPYS		5
#define NUM_OF_THREADS		4

struct mapped_dict
{
	char *m_buff;
	size_t m_buff_size;
	size_t m_num_words;
};


static mapped_dict_ty *CreateDictionaryBuffIMP(void);
static void CountNZeroingWordsIMP(mapped_dict_ty *dict_);
static void **CreatePtrsWordsBuffIMP(mapped_dict_ty *dict_);
static void DestroyIMP(mapped_dict_ty *dict_, void **words_buff_, void **multi_buff_);
static void **MakeCopiesIMP(void **buff_, size_t num_of_words_);

static void ShuffleArrayIMP(void **buff_, size_t size_);
int ShuffleCmpIMP(const void *arg1, const void *arg2);

static void *SortWithThreadsIMP(void **buff_, size_t size_);
static void *SortThreadIMP(void *param_);
int StrCmpIMP(const void *arg1, const void *arg2);

static void PrintDictIMP(void **buff_, size_t size_buff_);
static void PrintToFileIMP(void **buff_, void **org_, size_t size_buff_);


/*******************************************************************************
********************************** ShuffleNSortDict ***************************/

void **ShuffleNSortDict(void)
{
	mapped_dict_ty *dict = NULL;
	void **words_buff = NULL;
	void **multi_buff = NULL;
	void **org_buff = NULL;

	dict = CreateDictionaryBuffIMP();

	words_buff = CreatePtrsWordsBuffIMP(dict);
	org_buff = CreatePtrsWordsBuffIMP(dict);

	multi_buff = MakeCopiesIMP(words_buff, dict->m_num_words);
	org_buff = MakeCopiesIMP(words_buff, dict->m_num_words);

	ShuffleArrayIMP(multi_buff, dict->m_num_words);

	SortWithThreadsIMP(multi_buff, dict->m_num_words);
	PrintToFileIMP(multi_buff, org_buff, dict->m_num_words);


	DestroyIMP(dict, words_buff, multi_buff);

	return NULL;
}



static mapped_dict_ty *CreateDictionaryBuffIMP(void)
{
	mapped_dict_ty *dict = NULL;
	struct stat file_info = {0};
	int ret_err = 0;
	int dict_fd = 0;

	dict = malloc(sizeof(mapped_dict_ty));
	RETURN_IF_BAD(dict, "Allocation Failure", NULL);

	/* Get a file descriptor to the dictionary file */
	dict_fd = open(DICT_FILE_PATH, O_RDONLY);
	if (dict_fd < 0)
	{
		fputs("CreateDictionaryBuffIMP: Error Opening file for reading", stderr);
		return NULL;
	}


	/* Get statistics about the file using stat struct and function */
	ret_err = fstat(dict_fd, &file_info);
	if (ret_err < 0)
	{
		fputs("CreateDictionaryBuffIMP: Error getting the file size", stderr);
		return NULL;
	}

	assert((0 != file_info.st_size) && "CreateDictionaryBuffIMP: Error, File is Empty");


	dict->m_buff_size = file_info.st_size;

	/* Get pointer to the desired file, In Write mode */
	dict->m_buff = mmap(NULL, file_info.st_size, PROT_WRITE, MAP_PRIVATE, dict_fd, 0);
	if (dict->m_buff == MAP_FAILED)
	{
		close(dict_fd);
		fputs("CreateDictionaryBuffIMP: Error Mapping the file", stderr);
		return NULL;
	}


	/* Change each new line character to null terminator, and count all words */
	CountNZeroingWordsIMP(dict);


	ret_err = close(dict_fd);
	if (ret_err < 0)
	{
		fputs("CreateDictionaryBuffIMP: Error Closing file", stderr);
	}

	return dict;
}

static void CountNZeroingWordsIMP(mapped_dict_ty *dict_)
{
	char *runner = dict_->m_buff;
	size_t num_of_words_ = 0;

	while (*runner != '\0')
	{
		while (*runner != '\n')
		{
			++runner;
		}
		*runner = '\0';

		++num_of_words_;
		++runner;
	}

	dict_->m_num_words = num_of_words_;
}


static void **CreatePtrsWordsBuffIMP(mapped_dict_ty *dict_)
{
	void **words_buff = NULL;
	char *runner = NULL;
	size_t num_of_words_ = 0;
	size_t i = 0;

	ASSERT_NOT_NULL(dict_, "Dictionary ptr is not valid");

	runner = dict_->m_buff;
	num_of_words_ = dict_->m_num_words;
	
	words_buff = (void **)malloc(num_of_words_ * sizeof(size_t));
	RETURN_IF_BAD(words_buff, "Allocation Failure", NULL);

	for (i = 0; i < num_of_words_; ++i)
	{
		words_buff[i] = runner;

		while (*runner != '\0')
		{
			++runner;
		}

		++runner;
	}

	return words_buff;
}


static void DestroyIMP(mapped_dict_ty *dict_, void **words_buff_, void **multi_buff_)
{
	int ret_err = 0;

	/* Unmap pointer to file */
	ret_err = munmap(dict_->m_buff, dict_->m_buff_size);
	if (ret_err != 0)
	{
		fputs("CreateDictionaryBuffIMP: Error UnMapping the file", stderr);
	}

	free(dict_);
	free(multi_buff_);
	free(words_buff_);
}


static void **MakeCopiesIMP(void **buff_, size_t num_of_words_)
{
	void **multi_buff = NULL;
	void **run_multi_buff = NULL;
	size_t size_buff = (num_of_words_ * sizeof(void *));
	size_t size_multi_buff = size_buff * NUM_OF_CPYS;
	size_t i = 0;

	ASSERT_NOT_NULL(buff_, "Dictionary ptr is not valid");

	multi_buff = (void **)malloc(size_multi_buff);
	RETURN_IF_BAD(multi_buff, "Allocation Failure", NULL);

	run_multi_buff = multi_buff;

	for (i = 0; i < NUM_OF_CPYS; ++i)
	{
		/* Copy original buff to the multi_buff */
		memmove(run_multi_buff, buff_, size_buff);
		
		/* printf("%p\n", *(char **)run_multi_buff); */

		run_multi_buff += num_of_words_;
	}

	return multi_buff;
}


static void ShuffleArrayIMP(void **buff_, size_t size_)
{
	qsort(buff_, size_, sizeof(void *), ShuffleCmpIMP);
}

int ShuffleCmpIMP(const void *arg1, const void *arg2)
{
	UNUSED(arg1);
	UNUSED(arg2);

	srand(time(NULL));
	return (rand() % 3) - 1;
}


/*******************************************************************************
**************************** Sort With Threads ********************************/
typedef struct buff_range
{
	char **m_buff;
	size_t m_range;
} buff_range_ty;


static void *SortWithThreadsIMP(void **dict_, size_t num_words)
{
	buff_range_ty *to_sort = NULL;
	pthread_t *thread_id = NULL;
	size_t i = 0;
	int remainder = 0;
	int ret_stat = 0;

	ASSERT_NOT_NULL(dict_, "ptr is not valid");

	to_sort = (buff_range_ty *)calloc(NUM_OF_THREADS, sizeof(buff_range_ty));
	RETURN_IF_BAD(to_sort, "Allocation Failure", NULL);
	
	thread_id = (pthread_t *)calloc(NUM_OF_THREADS, sizeof(pthread_t));
	RETURN_IF_BAD(thread_id, "Allocation Failure", NULL);


	for (i = 0; i < NUM_OF_THREADS - 1; ++i)
	{
		to_sort[i].m_range = num_words / NUM_OF_THREADS;
		to_sort[i].m_buff = (char **)dict_ + (i * to_sort[i].m_range);

		ret_stat = pthread_create(&thread_id[i], NULL, SortThreadIMP, &to_sort[i]);

		if (0 != ret_stat)
		{
			return NULL;
		}
	}

	remainder = num_words % NUM_OF_THREADS;
	
	/* Calculate the last thread in case there is a remainder */
	to_sort[i].m_buff = (char **)dict_ + (i * to_sort[i].m_range);
	to_sort[i].m_range += remainder;

	ret_stat = pthread_create(&thread_id[i], NULL, SortThreadIMP, &to_sort[i]);

	if (0 != ret_stat)
	{
		return NULL;
	}

	/* Continue only when all threads are finished */
	for (i = 0; i < NUM_OF_THREADS - 1; ++i)
	{
		ret_stat = pthread_join(thread_id[i], NULL);

		if (0 != ret_stat)
		{
			return NULL;
		}
	}

	free(to_sort);
	free(thread_id);

	return dict_;
}

static void *SortThreadIMP(void *param_)
{
	qsort(((buff_range_ty *)param_)->m_buff, ((buff_range_ty *)param_)->m_range, sizeof(void *), StrCmpIMP);

	UNUSED(param_);

	pthread_exit(NULL);
}

int StrCmpIMP(const void *arg1, const void *arg2)
{
	return strcmp((char *)arg1, (char *)arg2);
}


/*******************************************************************************
**************************** Auxilary Functions *******************************/
static void PrintDictIMP(void **buff_, size_t size_buff_)
{
	size_t i = 0;

	for (i = 0; i < size_buff_; i++)
	{
		printf("%s\n", (char *)buff_[i]);
	}
}
static void PrintToFileIMP(void **buff_, void **org_, size_t size_buff_)
{
	size_t i = 0;
	FILE *filep = NULL;

	filep = fopen("log.txt", "w");

	for (i = 0; i < size_buff_; ++i)
	{
		fputs(buff_[i], filep);
		fputs("\t\t", filep);
		fputs(org_[i], filep);
		fputs("\n", filep);
	}

	fclose(filep);
}