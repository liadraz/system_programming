
/*******************************************************************************
******************************* - FS_POOL - ************************************
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Tests Fixed-Size Allocator
*	AUTHOR 			Liad Raz
*	REVIEWER		Sam
*	DATE			11/30/21
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts */
#include <stddef.h>		/* size_t */
#include <stdlib.h>		/* malloc, free*/


#include "utilities.h"
#include "fs_pool.h"

void TestFSPoolInit(void);
void TestFSPoolAllocate(void);
void TestFSPoolFree(void);
void TestFSPoolCountAvailable(void);
void TestFSPoolCalculateMemorySize(void);

static fs_pool_ty *CreateMemoryPool(void);


int main(void)
{
	PRINT_MSG(\n--- Tests Fixed-Size Allocator ---\n);
	
	TestFSPoolInit();
	TestFSPoolAllocate();
	TestFSPoolFree();
	TestFSPoolCountAvailable();
	TestFSPoolCalculateMemorySize();

	return 0;
}

void TestFSPoolInit(void)
{
	fs_pool_ty *pool = NULL;
	size_t pool_size = 54; 		/* ==> 	num_blocks = 3; block_size = 10; */
	size_t block_size = 10;
	void *memory = NULL;
	
	memory = malloc(54);
	
	if (memory == NULL)
	{
		PRINT_MSG("TestFSPoolInit: Allocation Failed");
		return;
	}
	
	pool = FSPoolInit(memory, pool_size, block_size);
	if ((void *)pool == memory)
	{
		GREEN;
		PRINT_STATUS_MSG(Test Init: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test Init: FAILED);
		DEFAULT;
	}
	
	free(memory);
}


void TestFSPoolAllocate(void)
{
	fs_pool_ty *pool = CreateMemoryPool();
	int test_counter = 0;
	void *allocated_ptr = NULL;
	
	allocated_ptr = FSPoolAllocate(pool);
	if (*(size_t *)allocated_ptr == 26 && *(size_t *)pool == 26)
	{ ++test_counter; }

	allocated_ptr = FSPoolAllocate(pool);
	if (*(size_t *)allocated_ptr == 36)
	{ ++test_counter; }
	
	allocated_ptr = FSPoolAllocate(pool);
	if (*(size_t *)allocated_ptr == 46)
	{ ++test_counter; }
	
	allocated_ptr = FSPoolAllocate(pool);
	if (*(size_t *)allocated_ptr == 56)
	{ ++test_counter; }
	
	allocated_ptr = FSPoolAllocate(pool);
	if (*(size_t *)allocated_ptr == 66 && *(size_t *)pool == 66)
	{ ++test_counter; }
	
	/* Test allocate more than mem pool can hold */
	allocated_ptr = FSPoolAllocate(pool);
	allocated_ptr = FSPoolAllocate(pool);
	
	if (allocated_ptr == NULL)
	{ ++test_counter; }
	
	if (test_counter == 6)
	{
		GREEN;
		PRINT_STATUS_MSG(Test Allocate: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test Allocate: FAILED);
		DEFAULT;
	}
	
	free(pool);
}


void TestFSPoolFree(void)
{
	fs_pool_ty *pool = CreateMemoryPool();
	size_t *allocated_ptr1 = NULL;
	size_t *allocated_ptr2 = NULL;
	int test_counter = 0;
	
	allocated_ptr1 = FSPoolAllocate(pool);
	allocated_ptr2 = FSPoolAllocate(pool);
	
	*allocated_ptr1 = 111;
	*allocated_ptr2 = 222;
	
	FSPoolFree(pool, allocated_ptr1);
	
	if (*allocated_ptr1 == 36 && *(size_t *)pool == 16)
	{ ++test_counter; }
	
	if (test_counter == 1)
	{
		GREEN;
		PRINT_STATUS_MSG(Test Free: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test Free: FAILED);
		DEFAULT;
	}
	
	free(pool);
}


void TestFSPoolCountAvailable(void)
{
	fs_pool_ty *pool = CreateMemoryPool();
	size_t *allocated_ptr1 = NULL;
	size_t *allocated_ptr2 = NULL;
	int test_counter = 0;
	
	if (FSPoolCountAvailable(pool) == 5)
	
	allocated_ptr1 = FSPoolAllocate(pool);
	FSPoolFree(pool, allocated_ptr1);
	allocated_ptr1 = FSPoolAllocate(pool);
	allocated_ptr1 = FSPoolAllocate(pool);
	
	{ ++test_counter; }
	if (FSPoolCountAvailable(pool) == 3)
	{ ++test_counter; }
 

	if (test_counter == 2)
	{
		GREEN;
		PRINT_STATUS_MSG(Test Count Available: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test Count Available: FAILED);
		DEFAULT;
	}
	
	free(pool);
}


void TestFSPoolCalculateMemorySize(void)
{
	size_t num_blocks = 5;
	size_t block_size = 12;
	size_t ret = 0;
	
	ret = FSPoolCalculateMemorySize(num_blocks, block_size);
	if (ret == 84)
	{
		GREEN;
		PRINT_STATUS_MSG(Test Calc Memory Size: SUCCESS);
		DEFAULT;
	}
	else
	{
		RED;
		PRINT_STATUS_MSG(Test Calc Memory Size: FAILED);
		DEFAULT;
	}
}



/*-------------------------------Side Functions ------------------------------*/
/* 
Cretaes memory pool:
			managmement 16
			End block	8
			actual MEM	50
						===
			Total	 	74
			block size 	10
			blocks		5
*/
static fs_pool_ty *CreateMemoryPool(void)
{
	fs_pool_ty *pool = NULL;
	size_t *memory = NULL;
	size_t block_size = 10;
	size_t num_blocks = 5;
	size_t pool_size = 0;
	
	pool_size = FSPoolCalculateMemorySize(num_blocks, block_size); 
	
	memory = (size_t *)malloc(pool_size);
	if (memory == NULL)
	{
		PRINT_MSG("CreateMemoryPool: Allocation Failed");
		return NULL;
	}
	
	pool = FSPoolInit(memory, pool_size, block_size);
	
	return pool;
}


