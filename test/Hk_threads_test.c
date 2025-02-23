/*******************************************************************************
					- HK_THREADS -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Test
*	AUTHOR 		Liad Raz
*	REVIEWER		Omri
*	DATE			02 Feb 2022
*
*******************************************************************************/


#include <stdio.h>		/* printf, puts, putchar, size_t */

#include "utilities.h"
#include "Hk_threads.h"

void TestFunc(void);


int main(void)
{
	size_t num = 123456789012;
	size_t threads = 3;
	int option = 0;

	PRINT_MSG(\n\t--- Tests Hk_threads ---\n);

	puts("Please, Choose Exercise Number");
	if (0 == scanf("%d", &option))
	{
		return 1;
	}
	
	switch (option)
	{
		case 1:
		{
			Create100KThreadsExe1();
			break;
		}
		case 2:
		{
			Create100KThreadsExe2();
			break;
		}
		case 3:
		{
			Create100KThreadsExe3();
			break;
		}
		case 4:
		{
			while (threads <= 15)
			{
				puts("***************************************************");
				Create100KThreadsExe4(num, threads);
				puts("***************************************************");
				++threads;
			}
			break;
		}
		case 410:
		{
			puts("***************************************************");
			Create100KThreadsExe4(num, 6);
			puts("***************************************************");
			++threads;
			break;
		}
		case 5:
		{
			Create100KThreadsExe5(num);
			break;
		}
	default:
		break;
	}

	NEW_LINE;
	NEW_LINE;
	
	return 0;
}




