
/*******************************************************************************
******************************* - FS_POOL - ************************************
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		API Fixed-Size Allocator
*	AUTHOR 			Liad Raz
*	REVIEWER		Sam
*	DATE			11/30/21
*	FILES			fs_pool.c fs_pool_test.c fs_pool.h
*
*******************************************************************************/

#ifndef __FS_POOL_H__
#define __FS_POOL_H__

typedef struct fs_pool fs_pool_ty;

/*******************************************************************************
* DESCRIPTION 	Initiate free list (memory-pool) which divided to fixed size blocks.
* RETURN		Undefined behavior when 'memory' is not allocated.
* IMPORTANT	 	Minimum block size must be 8 bytes (In 64 bit operation).
				Fixed-size allocator requires extra memory for managment purposes
				(Use CalculateMemorySize function to get it).
*				

* Time Complexity: 	O(amount_of_blocks)
*******************************************************************************/
fs_pool_ty *FSPoolInit(void *memory, size_t pool_size_bytes, size_t block_size_bytes);


/*******************************************************************************
* DESCRIPTION	Allocate one block of memory.
* RETURN	 	Null when memory-pool is full.

* Time Complexity: 	O(1)
*******************************************************************************/
void *FSPoolAllocate(fs_pool_ty *mem_pool);


/*******************************************************************************
* DESCRIPTION	Frees one block from memory-pool which provided by the user.
* IMPORTANT	 	Undefined behavior 
				- when the provided block is NULL
				- when block address is not in memory-pool addresses range.

* Time Complexity: 	O(1)
*******************************************************************************/
void FSPoolFree(fs_pool_ty *mem_pool, void *block);


/*******************************************************************************
* DESCRIPTION	Counts how many free blocks exist in memory-pool.

* Time Complexity: 	O(n)
*******************************************************************************/
size_t FSPoolCountAvailable(const fs_pool_ty *mem_pool);


/*******************************************************************************
* DESCRIPTION	Provide the size of memory required to initiate the memory-pool.
* IMPORTANT	 	Fixed-size allocator requires extra memory for managment purposes.

* Time Complexity: 	O(1)
*******************************************************************************/
size_t FSPoolCalculateMemorySize(size_t num_blocks, size_t block_size_bytes);


#endif /* __FS_POOL_H__ */
