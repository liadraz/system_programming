/*******************************************************************************
					- SHUFFLE_SORT_DICT -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		API
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			20 Feb 2022
*	FILES			shuffle_sort_dict.c shuffle_sort_dict_test.c shuffle_sort_dict.h
* 
*******************************************************************************/

#ifndef __SHUFFLE_SORT_DICT_H__
#define __SHUFFLE_SORT_DICT_H__

typedef struct mapped_dict mapped_dict_ty;

/*******************************************************************************
* DESCRIPTION	Sort a dictionary file using Multiple threads.
* IMPORTANT	 	
*
* Time Complexity 	O(n)
*******************************************************************************/
void **ShuffleNSortDict(void);


/*******************************************************************************
* DESCRIPTION	Sort a dictionary file using Multiple threads.
* IMPORTANT	 	
*
* Time Complexity 	O(n)
*******************************************************************************/
void UnLoadIMP(mapped_dict_ty *dict_);


#endif /* __SHUFFLE_SORT_DICT_H__ */

