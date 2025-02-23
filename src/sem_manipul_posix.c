
/*******************************************************************************
					- SEMAPHORE -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Implement - Semaphore Manipulation using POSIX API
*	AUTHOR 		Liad Raz
*	REVIEWER		Yanal
*	DATE			06 Feb 2022
*
*	gc src/semaphore.c test/semaphore_test.c -I ../ -I include/semaphore.h
*******************************************************************************/

#include <stdio.h>			/* puts, fgets, sscanf */
#include <assert.h>			/* assert */
#include <semaphore.h>			/* sem_t,  SEM_FAILED. sem_open,
						sem_getvalue */
#include <sys/stat.h>			/* O_RDWR */
#include <fcntl.h>			/* O_CREAT */
#include <errno.h>			/* errno */

#include "utilities.h"
#include "sem_manipul_posix.h"

#define MAX_NAME	40

/*******************************************************************************
*********************************** Impl **************************************/
void SemManipulation(char *sem_name_)
{
	sem_t *semaphore = NULL;
	char undo[MAX_NAME] = {0};
	char input[MAX_NAME] = {0};
	char command = '\0';
	int number = 0;
	int sem_value = 0;
	int is_run = 1;
	int ret = 0;

	assert(NULL != sem_name_);

	semaphore = sem_open(sem_name_, O_CREAT | O_EXCL, O_RDWR, 0);
	if (semaphore == SEM_FAILED)
	{
		printf("sem_open: Value of errno %d\n", errno);
		return;
	}

	while (is_run)
	{
		puts("Enter a Command:");
		if (!fgets(input, MAX_NAME, stdin))
		{
			puts("\nRead input failed");
			return;	
		}

		if (!sscanf(input, "%c %d %s", &command, &number, undo))
		{
			puts("\nRead input failed");
			return;	
		}
		
		switch ((int)command)
		{
			case 'd':
			case 'D':
			{
				while (number)
				{
					sem_wait(semaphore);
					--number;
				}
				number = 0;
				break;
			}
			case 'i':
			case 'I':
			{
				while (number)
				{
					sem_post(semaphore);
					--number;
				}
				break;
			}
			case 'v':
			case 'V':
			{
				ret = sem_getvalue(semaphore, &sem_value);
				printf("Semaphore Current Value Is \t%d\n", sem_value);

				if (ret != 0)
				{
					puts("sem_getvalue: Error Failed");
					printf("sem_getvalue: Value of errno %d\n", errno);
					return;
				}
				break;
			}
			case 'h':
			case 'H':
			{
				puts("\t(D)Decrease, \t(I)Increment, \t(V)GetValue, \t(X)Exit");
				break;
			}
			case 'x':
			case 'X':
			{
				is_run = 0;
				puts("\n\n\t\tGOODBYE");
				break;
			}
			default:
				puts("Invalid Input");
				break;

			puts("\n");
		}
	}

	ret = sem_close(semaphore);
	if (ret != 0)
	{
		puts("sem_close: Error Failed");
		printf("sem_close: Value of errno %d\n", errno);
		return;
	}

	ret = sem_unlink(sem_name_);
	if (ret != 0)
	{
		puts("sem_unlink: Error Failed");
		printf("sem_unlink: Value of errno %d\n", errno);
		return;
	}
}