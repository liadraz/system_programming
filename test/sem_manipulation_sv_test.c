/*******************************************************************************
					- SEM_MANIPULATION_SV -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Main - Semaphore Manipulation using System V API
*	AUTHOR 		Liad Raz
*	REVIEWER		
*	DATE			06 Feb 2022
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts, putchar, size_t */

#include "utilities.h"
#include "sem_manipulation_sv.h"

#define InputSize	20


int main(int argc, char *argv[])
{
	char *name = NULL;

	puts("\n\t~~~ Semaphore Manipulation ~~~");
	puts("\t\t~~~ System V API ~~~\n");

	name = (char *)calloc(InputSize, sizeof(char));
	if (name == NULL)
	{
		return 1;
	}

	/* Check if user provided a second argument */
	if (argc < 2)
	{
		puts("Please, Provide a name to the semaphore: ");
		if (!scanf("%s", name))
		{
			puts("\nRead input failed");
			return 1;
		}
	}

	/* In case user did provided, copy it to the allocated stinrg */
	if (NULL != argv[1])
	{
		strcpy(name, argv[1]);
	}

	SemManipulation(name);

	NEW_LINE;
	NEW_LINE;
	
	free(name);
	return 0;
}


