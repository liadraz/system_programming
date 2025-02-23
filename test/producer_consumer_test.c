/*******************************************************************************
					- PRODUCER_CONSUMER -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Test
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			07 Feb 2022
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts, putchar, size_t */

#include "utilities.h"
#include "producer_consumer.h"


int main(void)
{
	PRINT_MSG(\n\t--- PRODUCER CONSUMER ---\n);
	
	ProducerConsumerExe1();
	NEW_LINE;
	ProducerConsumerExe2();
	NEW_LINE;
	

	ProducerConsumerExe3();
	NEW_LINE;

	ProducerConsumerExe6();
	NEW_LINE;

	
	NEW_LINE;
	
	return 0;
}



