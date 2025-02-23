/*******************************************************************************
					- PRODUCER_CONSUMER -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		API
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			07 Feb 2022
*	FILES			producer_consumer.c producer_consumer_test.c producer_consumer.h
* 
*******************************************************************************/

#ifndef __PRODUCER_CONSUMER_H__
#define __PRODUCER_CONSUMER_H__


/*******************************************************************************
* DESCRIPTION	Create one producer thread and one Consumer thread.
			- Producer will assign 1 number to 1000 elements array.
			- Consumer will sum the whole elements of the array.
			Both sides will use (atomic increment/decremenet) busy wait.
* 
*******************************************************************************/
void ProducerConsumerExe1(void);


/*******************************************************************************
* DESCRIPTION	Create multiple producers and consumers.
			Use linked list container and 
			one mutex to lock the critical sections.
			- Producer will increment one node a time.
			- Consumer will decrement one node a time.
*
*******************************************************************************/
void ProducerConsumerExe2(void);


/*******************************************************************************
* DESCRIPTION	Create multiple producers and consumers.
			Use linked list container, one mutex
			and semaphore to count the available nodes.
*
*******************************************************************************/
void ProducerConsumerExe3(void);


/*******************************************************************************
* DESCRIPTION	
* RETURN	 	
* IMPORTANT	 	
*
* Time Complexity 	O(n)
*******************************************************************************/
void ProducerConsumerExe6(void);


#endif /* __PRODUCER_CONSUMER_H__ */

