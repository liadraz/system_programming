/*******************************************************************************
					- WATCHDOG -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		RUN Watchdog
*	AUTHOR 			Liad Raz
*	REVIEWER		Ronen
*	DATE			14 Feb 2022
*
*******************************************************************************/

#include <stdio.h>		/* printf, puts, putchar, size_t */
#include <unistd.h>		/* sleep */

#include "utilities.h"
#include "watchdog.h"


int main(int argc, char *argv[])
{
	size_t i = 0;
	size_t size = 20;

	PRINT_MSG(\n\t--- RUN Watchdog ---\n);


	if (0 != KeepMeAlive(argc, argv))
	{
		return 1;
	}


	for (i = 0; i < size; ++i)
	{
		puts("Running");
		sleep(1);

		if (i > 5)
		{
			if (0 != DoNotResuscitate())
			{
				return 1;
			}
		}
	}


	UNUSED(argc);
	NEW_LINE;

	return 0;
}



