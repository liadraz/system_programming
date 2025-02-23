/*******************************************************************************
*************************** - VS_Allocator - ***********************************
************************** System Programming **********************************
*
*	DESCRIPTION		Tests Variable Size Allocator
*	AUTHOR 			Liad Raz
*	REVIEWER		Ira
*	DATE			02 Dec 2021
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts */
#include <stddef.h>		/* size_t */
#include <stdlib.h>		/* malloc, free */

#include "utilities.h"
#include "vs_pool.h"

#define END_BLOCK_SIZE		SIZE_OF_DOUBLE
#define END_FLAG			3 /* Must be number less than BYTE */

void TestVSPoolInit(void);
void TestVSPoolAlloc(void);
void TestVSPoolFree(void);
void TestVSPoolLargestChunkAvailable(void);

static void PrintPool(vs_pool_ty *pool_);
static ptrdiff_t *ToMd(void *current_);

int main(void)
{
	PRINT_MSG(\n\t--- Tests Variable-Size Allocator ---\n);
	
	TestVSPoolInit();
	TestVSPoolAlloc();
	TestVSPoolFree();
	TestVSPoolLargestChunkAvailable();

	NEW_LINE;
	NEW_LINE;
	return 0;
}

void TestVSPoolInit(void)
{
	void *mem = NULL;
	vs_pool_ty *pool = 0;
	size_t pool_size = 56; 	/* [40,	0,	0,	0,	0,	0, 3] */
	void *end_pool = 0;
	size_t tcount = 0;

	mem = malloc(pool_size);
	if (NULL == mem)
	{
		puts("Memory Allocation Failed");
		return;
	}
	
	pool = VSPoolInit(mem, pool_size);
	
	if (*(size_t *)pool == 40)
	{ ++tcount; }
	
	if (*(size_t *)((char *)pool + 48) == 3)
	{ ++tcount; }
	
	if (tcount == 2)
	{
		GREEN;
		PRINT_STATUS_MSG(Test VSPoolInit: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test VSPoolInit: FAILED);
		DEFAULT;
	}
	
	free(mem);
}

void TestVSPoolAlloc(void)
{
	void *mem = NULL;
	vs_pool_ty *pool = 0;
	size_t pool_size = 64;		/* [48,	0,	0,	0,	0,	0,	0, 3] */
	void *block_alloc1 = NULL;
	void *block_alloc2 = NULL;
	void *block_alloc3 = NULL;
	void *block_alloc4 = NULL;
	size_t tcount = 0;
	
	mem = malloc(pool_size);
	if (NULL == mem)
	{
		puts("Memory Allocation Failed");
		return;
	}

	pool = VSPoolInit(mem, pool_size);
	PrintPool(pool);
	
	/* Test First Case => Free Block size fits */
	block_alloc1 = VSPoolAlloc(pool, 5);
	PrintPool(pool);
	
	if (*(size_t *)((char *)block_alloc1 + 48) == END_FLAG)
	{ ++tcount; }
	
	/* Test Second Case => Block is not free */
	block_alloc2 = VSPoolAlloc(pool, 12);
	PrintPool(pool);
	
	if (*(size_t *)((char *)block_alloc2 + 32) == END_FLAG)
	{ ++tcount; }
	
	/* Test Third Case => Allocated block does not fit invoke merge */
	VSPoolFree(block_alloc2);
	PrintPool(pool);
	block_alloc3 = VSPoolAlloc(pool, 24);
	PrintPool(pool);
	
	if (*(size_t *)((char *)block_alloc3 + 32) == END_FLAG)
	{ ++tcount; }
	
	/* Test Fourth Case => test when list is full */	
	block_alloc4 = VSPoolAlloc(pool, 50);
	PrintPool(pool);
	
	if (block_alloc4 == NULL)
	{ ++tcount; }
	
	if (tcount == 4)
	{
		GREEN;
		PRINT_STATUS_MSG(Test PoolAlloc: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test PoolAlloc: FAILED);
		DEFAULT;
	}
	
	free(mem);
}

void TestVSPoolFree(void)
{
	void *mem = NULL;
	vs_pool_ty *pool = 0;
	size_t pool_size = 64;			/* [48,	0,	0,	0,	0, 0, 3] */
	void *block_alloc1 = NULL;
	size_t tcount = 0;
	
	mem = malloc(pool_size);
	if (NULL == mem)
	{
		puts("Memory Allocation Failed");
		return;
	}

	pool = VSPoolInit(mem, pool_size);
	PrintPool(pool);
	
	block_alloc1 = VSPoolAlloc(pool, 24);
	PrintPool(pool);

	VSPoolFree(block_alloc1);
	PrintPool(pool);
	
	if (*(size_t *)((char *)block_alloc1 + 24) == 16)
	{ ++tcount; }
	
	if (tcount == 1)
	{
		GREEN;
		PRINT_STATUS_MSG(Test VSPoolFree: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test VSPoolFree: FAILED);
		DEFAULT;
	}
	
	free(mem);
}


void TestVSPoolLargestChunkAvailable(void)
{
	void *mem = NULL;
	vs_pool_ty *pool = 0;
	size_t pool_size = 64;			/* [48,	0,	0,	0,	0, 0, 3] */
	void *block_alloc1 = NULL;
	void *block_alloc2 = NULL;
	void *block_alloc3 = NULL;
	size_t tcount = 0;
	
	mem = malloc(pool_size);
	if (NULL == mem)
	{
		puts("Memory Allocation Failed");
		return;
	}
	
	pool = VSPoolInit(mem, pool_size);

	block_alloc1 = VSPoolAlloc(pool, 8);
	block_alloc2 = VSPoolAlloc(pool, 16);
	block_alloc3 = VSPoolAlloc(pool, 8);
	VSPoolFree(block_alloc2);
	
	if (VSPoolLargestChunkAvailable(pool) == 16)
	{ ++tcount; }
	PrintPool(pool);
	
	VSPoolFree(block_alloc3);
	PrintPool(pool);
	if (VSPoolLargestChunkAvailable(pool) == 32)
	{ ++tcount; }
	
	if (tcount == 2)
	{
		GREEN;
		PRINT_STATUS_MSG(Test LargestChunkAvailable: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test LargestChunkAvailable: FAILED);
		DEFAULT;
	}
	
	free(mem);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Side Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static void PrintPool(vs_pool_ty *pool_)
{
	ptrdiff_t *runner = (void *)pool_;
	
	while (*runner != END_FLAG)
	{
		printf("%ld\t", *runner); 
		runner = (ptrdiff_t *)((char *)runner + SIZE_OF_DOUBLE);
	}
	
	printf("End(%ld)", *runner); 

	NEW_LINE;
}



