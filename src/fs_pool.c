
/*******************************************************************************
******************************* - FS_POOL - ************************************
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Fixed-Size Allocator Implementation
*	AUTHOR 			Liad Raz
*	REVIEWER		Sam
*	DATE			11/30/21
*
*******************************************************************************/
#include <stdio.h>
#include <stddef.h>		/* size_t, ptrdiff_t */
#include <assert.h>		/* assert */

#include "utilities.h"
#include "fs_pool.h"

#define ASSERT_NOT_NULLFS(ptr)	assert (NULL != ptr \
								&& "Fixed-Size Allocator is not allocated");

#define POOL_STRUCT_SIZE 	sizeof(fs_pool_ty)
#define END_BLOCK	 		sizeof(size_t)

struct fs_pool
{
    size_t head;
	DEBUG_MODE(size_t pool_size;)
};

static size_t CalcAmountOfBlocks(size_t pool_size_, size_t block_size_);
static void *ToNextData(void *current_, size_t block_size_);
/*static void AssignOffsetData(void *pool_, size_t block_size_);*/

/*******************************************************************************
**************************** FSPool Init **************************************/
fs_pool_ty *FSPoolInit(void *th_, size_t pool_size_, size_t block_size_)
{
	fs_pool_ty *mem_pool = NULL;
	void *runner = NULL;
	size_t amount_blocks = 0;
	size_t i = 0;
	
	assert (NULL != th_ 
	&& "FSPoolInit: Invalild Pointer");

	assert (SIZE_OF_DOUBLE <= block_size_ 
	&& "FSPoolInit: block size cannot be less than 8 BYTES");

	assert (0 != pool_size_ 
	&& "FSPoolInit: pool size cannot be zero");
	
	/* Store in head member the offset to first free block */
	mem_pool = th_;
	mem_pool->head = POOL_STRUCT_SIZE;
	
	/* jump to first block address from maintenance struct */
	runner = ToNextData(th_, POOL_STRUCT_SIZE);
	
	/* Calculate the number of free blocks */
	amount_blocks = CalcAmountOfBlocks(pool_size_, block_size_);	
	
	for (i = 1; i <= amount_blocks; ++i)
	{
		/* Each block will hold the offset value to the next block */
		*(size_t *)runner = mem_pool->head + block_size_ * i;
		
		/* jump to next block offset */
		runner = ToNextData(runner, block_size_);
	}
	
	/* Last block will hold NULL to indicate End of list */
	*(size_t *)runner = 0;
	
	/* init the memory pool size member */
	DEBUG_MODE(mem_pool->pool_size = pool_size_);

	return mem_pool;
}


/*******************************************************************************
**************************** FSPool Allocate **********************************/
void *FSPoolAllocate(fs_pool_ty *th_)
{
	void *ret_block = 0;
	
	ASSERT_NOT_NULLFS(th_);
	
	/* In case pool is full */
	if (th_->head == 0)
	{
		return NULL;
	}
	
	/* Jump to the free block head stores */
	ret_block = ToNextData(th_, th_->head);
	
	/* Update head value with the offset data return_block has */
	th_->head = *(size_t *)ret_block;
	
	return ret_block;
}


/*******************************************************************************
**************************** FSPool Free **************************************/
void FSPoolFree(fs_pool_ty *th_, void *to_free_)
{
	ASSERT_NOT_NULLFS(th_);
	
	assert (NULL != to_free_ 
	&& "FSPoolInit: Invalild Pointer");
	
	assert (*(size_t *)&th_ < *(size_t *)&to_free_ 
	&& *(size_t *)&th_ + th_->pool_size > *(size_t *)&to_free_ 
	&& "FSPoolInit: block address must be in range of memory pool");
	
	/* Assign to_free_block the offset value head holds */
	*(size_t *)to_free_ = th_->head;

	/* Change head offset value to be the provided block */
	th_->head = *(size_t *)&to_free_ - *(size_t *)&th_;
}

/*******************************************************************************
************************* FSPool Count Available ******************************/
size_t FSPoolCountAvailable(const fs_pool_ty *th_)
{
	void *runner = NULL;
	size_t counter = 0;
	
	ASSERT_NOT_NULLFS(th_);
	
	/* jump to the first free block */
	runner = ToNextData((void *)th_, *(size_t *)th_);
	
	/* iterate each data block until the End block */
	while (*(size_t *)runner != 0)
	{
		/* move runner to the next free block */
		runner = ToNextData((void *)th_, *(size_t *)runner);
		
		++counter;
	}
	
	return counter;
}


/*******************************************************************************
************************ FSPool Calculate Memory Size *************************/
size_t FSPoolCalculateMemorySize(size_t amount_blocks_, size_t block_size_)
{
	return (amount_blocks_ * block_size_) + POOL_STRUCT_SIZE + END_BLOCK;
}


/*******************************************************************************
***************************** Side Functions **********************************/
static size_t CalcAmountOfBlocks(size_t pool_size_, size_t block_size_)
{
	return ((pool_size_ - POOL_STRUCT_SIZE - END_BLOCK) / block_size_);
}

static void *ToNextData(void *current_, size_t block_size_)
{
	return (void *)((char *)current_ + block_size_);
}







