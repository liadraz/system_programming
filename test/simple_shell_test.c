/*******************************************************************************
					- SIMPLE_SHELL -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Test
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			30 Jan 2022
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts, putchar, size_t */

#include "utilities.h"
#include "simple_shell.h"

void TestFunc(void);

int main(void)
{
	PRINT_MSG(\n\t--- Tests simple_shell ---\n);
	
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


