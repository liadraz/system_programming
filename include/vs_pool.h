/*******************************************************************************
*************************** - VS_Allocator - ***********************************
************************** System Programming **********************************
*
*	DESCRIPTION		API Variable Size Allocator
*	AUTHOR 			Liad Raz
*	REVIEWER		Ira
*	DATE			02 Dec 2021
*	FILES			vs_pool.c vs_pool_test.c vs_pool.h
* 
*******************************************************************************/

#ifndef __VS_POOL_H__
#define __VS_POOL_H__

typedef struct vs_pool vs_pool_ty;

/*******************************************************************************
* DESCRIPTION	Initiate memory pool that can hold different block sizes.
* IMPORTANT	 	Undefined behavior 
				- when 'memory' is not allocated.
				- pool_size_in_bytes must be aligned to Double size.
*
* Time Complexity 	O(n)
*******************************************************************************/
vs_pool_ty *VSPoolInit(void *memory, size_t pool_size_in_bytes);


/*******************************************************************************
* DESCRIPTION	Allocate a block of memory the size provided by the user.
* RETURN		Null when memory-pool is full.
*
* Time Complexity	O(n)
*******************************************************************************/
void *VSPoolAlloc(vs_pool_ty *pool, size_t num_bytes);


/*******************************************************************************
* DESCRIPTION	Frees block from memory-pool which provided by the user.
* IMPORTANT	 	Undefined behavior 
				- when the provided block is NULL
				- when block address is not in memory-pool addresses range.
*
* Time Complexity	O(1)
*******************************************************************************/
void VSPoolFree(void *block);


/*******************************************************************************
* DESCRIPTION	Get the biggest block of avilable memory
* RETURN		0 when pool is full .
*
* Time Complexity	O(n)
*******************************************************************************/
size_t VSPoolLargestChunkAvailable(vs_pool_ty *pool);


#endif /* __VS_POOL_H__ */
