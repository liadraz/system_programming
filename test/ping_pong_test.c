/*******************************************************************************
					- PING_PONG -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Test
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			01 Feb 2022
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts, putchar, size_t */

#include "utilities.h"
#include "ping_pong.h"

void TestFunc(void);

int main(void)
{
	PRINT_MSG(\n\t--- Tests ping_pong ---\n);
	
	TestFunc();

	NEW_LINE;
	NEW_LINE;
	
	return 0;
}

void TestFunc(void)
{
	size_t tcounter = 0;

	if (th_ != NULL)
	{ ++tcounter; }
	
	PrintTestStatusIMP(tcounter, num_tests, "Function_Name");
}


