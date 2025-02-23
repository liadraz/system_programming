/*******************************************************************************
*************************** - VS_Allocator - ***********************************
************************** System Programming **********************************
*
*	DESCRIPTION		Implementatio of Variable Size Allocator
*	AUTHOR 			Liad Raz
*	REVIEWER		Ira
*	DATE			02 Dec 2021
*
*******************************************************************************/

#include <stddef.h>			/* ptrdiff_t */
#include <assert.h>			/* assert */

#include "utilities.h"		/* ASSERT_NOT_NULL, SIZE_OF_DOUBLE */
#include "vs_pool.h"

#define POOL_STRUCT_SIZE	sizeof(vs_pool_ty)
#define END_BLOCK_SIZE		SIZE_OF_DOUBLE
#define END_FLAG			3 	/* Must be number less than BYTE */

struct vs_pool
{
    ptrdiff_t available;
};


static size_t ActualMemSizeIMP(size_t pool_size_);
static void AllocateBlock(ptrdiff_t *current_, size_t block_size_);
static ptrdiff_t *MergeAvailableBlocks(ptrdiff_t *current_);
static size_t AddPadding(size_t block_size_);

static ptrdiff_t *ToFirstData(void *memory_);
static ptrdiff_t *ToNextData(void *current_, size_t block_size_);
static ptrdiff_t *ToMd(void *current_);

/*******************************************************************************
******************************** VSPoolInit ***********************************/
vs_pool_ty *VSPoolInit(void *memory_, size_t pool_size_)
{
	assert(NULL != memory_ 
	&& "VSPoolInit: Allocated Memory is not allocated");

	assert(pool_size_ % SIZE_OF_DOUBLE == 0 
	&& "VSPoolInit: pool_size is not aligned");

	/* Init First MetaData member with the actual value of avilable memory */
	*ToMd((ToFirstData(memory_))) = ActualMemSizeIMP(pool_size_);

	/* Jump to END block and init it with the value of END_FLAG */
	*ToMd((char *)memory_ + pool_size_) = END_FLAG;

	return (vs_pool_ty *)memory_;
}


/*******************************************************************************
******************************** VSPoolAlloc **********************************/
void *VSPoolAlloc(vs_pool_ty *th_, size_t num_bytes_)
{
	ptrdiff_t *metadata = NULL;
	ptrdiff_t *current = NULL;
	
	assert(NULL != th_
	&& "VSPoolAlloc: Memory Pool is not allocated");
	
	/* align amount of bytes to the size of double (=> 8 BYTES) */
	num_bytes_ = AddPadding(num_bytes_);

	/* Jump to first block position */
	current = ToFirstData(th_);
	
	/* iterate until the end block */
	while (END_FLAG != *ToMd(current))
	{
		/* Keep the position of meta_data */
		metadata = ToMd(current);

		/* Case: block is already allocated, jump to the next block */
		if (*metadata < 0)
		{
			current = ToNextData(current, (*ToMd(current)) * -1);
		}
		/* Case, Free Block is bigger than the to allocate block */
		else if (*metadata >= (ptrdiff_t)num_bytes_)
		{
			AllocateBlock(current, num_bytes_);
			return (void *)current;
		}
		/* Case, Free Block is smaller than the allocated block */
		else 
		{
			current = MergeAvailableBlocks(current);
		}
	}
	
	/* Case, when list is full return NULL */	
	return NULL;
}

/*******************************************************************************
************************** VSPool Free ****************************************/
void VSPoolFree(void *block_)
{
	assert(NULL != block_ && "Free: Block is not allocated");

	/* negate the block meta data */
	*ToMd(block_) *= -1;
}

/*******************************************************************************
************************* VSPool LargestChunkAvail ****************************/
size_t VSPoolLargestChunkAvailable(vs_pool_ty *th_)
{
	ptrdiff_t *current = NULL;
	size_t max_block = 0;
	 
	assert(NULL != th_
	&& "LargestChunkAvail: Memory Pool is not allocated");
	
	/* preserve the first block pointer */	
	current = ToFirstData(th_);

	/* Iterate until the ENToFirstDataD FLAG */	 	
 	while (END_FLAG != *ToMd(current))
 	{
		/* In case data block is allocated */
		if (*ToMd(current) < 0)
		{
			current = ToNextData(current, (*ToMd(current)) * -1);
			continue;
		}

 		/* merge adjacent available blocks */
 		MergeAvailableBlocks(current);
	
		/* In case bigger data block was found update max variable */
 		if ((ptrdiff_t)max_block < *ToMd(current))
 		{
			max_block = *ToMd(current);
 		}
 		
 		current = ToNextData(current, *ToMd(current));
 	}
 	
 	return max_block;
}


/*******************************************************************************
***************************** Side Functions **********************************/

/* calculate the available memory that left after subtracting meta data*/
static size_t ActualMemSizeIMP(size_t pool_size_)
{
	return pool_size_ - POOL_STRUCT_SIZE - END_BLOCK_SIZE;
}


static void AllocateBlock(ptrdiff_t *current_, size_t block_size_)
{
	ptrdiff_t *to_alloc = NULL;
	size_t org_metadata = *ToMd(current_);

	/* Change allocated MD value to negative the 'allocated block size' */
	*ToMd(current_) = -1 * block_size_;
	
	/* Case, when block_size_ equals to current meta data do not alocate  */	
	if (block_size_ == org_metadata)
	{
		return;
	}
	
	/* Jump to the next free block data */
	to_alloc = ToNextData(current_, block_size_);
	
	/* Calculate the allocated block MD value */
	*ToMd(to_alloc) = org_metadata - block_size_ - SIZE_OF_DOUBLE;
}


/* Merge two free blocks together. */
static ptrdiff_t *MergeAvailableBlocks(ptrdiff_t *current_)
{
	/* Jump to the next block */
	ptrdiff_t *next_data = ToNextData(current_, *ToMd(current_));
	 
	/* In case next block is free, merge it with the first */
	if (*ToMd(next_data) > 0 && *ToMd(next_data) != END_FLAG) 
	{
		*ToMd(current_) += *ToMd(next_data) + SIZE_OF_DOUBLE;
	}
	else
	{
		current_ = next_data;
	}
	
	return current_;
}


/* align allocated block to the size of double (=> 8 BYTES) */
static size_t AddPadding(size_t block_size_)
{
	return ((block_size_ + (SIZE_OF_DOUBLE - 1)) / SIZE_OF_DOUBLE) * SIZE_OF_DOUBLE;
}


/* Jump to the First position of Block Data */
static ptrdiff_t *ToFirstData(void *memory_)
{
	return (ptrdiff_t *)((char *)memory_ + POOL_STRUCT_SIZE);
}


/* Jump to the Next Block Data */
static ptrdiff_t *ToNextData(void *current_, size_t block_size_)
{
	return (ptrdiff_t *)((char *)current_ + (SIZE_OF_DOUBLE + block_size_));
}


/* Jump back to the point where MetaData starts */
static ptrdiff_t *ToMd(void *current_)
{
	return (ptrdiff_t *)((char *)current_ - SIZE_OF_DOUBLE);
}

