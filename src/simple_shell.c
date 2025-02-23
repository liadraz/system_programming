
/*******************************************************************************
					- SIMPLE_SHELL -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Implementation
*	AUTHOR 		Liad Raz
*	REVIEWER		Ira
*	DATE			30 Jan 2022
*
*******************************************************************************/

#include <stdio.h>			/* puts, scanf */
#include <string.h>			/* strcmp */
#include <sys/types.h>			/*  pid_t */
#include <sys/wait.h>			/*  wait */
#include <stdlib.h>			/* system, calloc, free */
#include <unistd.h>			/* execvp, fork */

/*#include "utilities.h"*/
/*#include "simple_shell.h"*/

static void RunSimpleShell(void);
static void ForkImp(void);
static void SystemImp(void);


/*******************************************************************************
****************************** SIMPLE_SHELL ***********************************/
int main(void)
{
	RunSimpleShell();

	return 0;
}


static void RunSimpleShell(void)
{
	char *user_input = NULL;

	user_input = (char *)calloc(40, sizeof(char));
	if (NULL == user_input)
	{
		puts("Allocation failed");
		return; 
	}

	puts("\n\t~ WELCOME TO SIMPLE SHELL ~\n");

	while (0 != strcmp(user_input, "exit"))
	{
		puts("\n\nEnter a running process method: \n\tfork, system, or exit");

		if (!scanf("%40s", user_input))
		{
			puts("read input failed");
			return;
		}
		
		/* In case user chose Fork or System */
		if (!strcmp(user_input, "fork"))
		{
			ForkImp();
		}
		else if (!strcmp(user_input, "system"))
		{
			SystemImp();
		}
		else if (!strcmp(user_input, "exit"))
		{
			continue;
		}
		else
		{
			puts("\tWrong Usage!");
		}
	}
	
}

static void ForkImp(void)
{
	char *argv[3] = {NULL};
	pid_t pid = 0;

	*argv = (char *)calloc(40, sizeof(char));
	if (NULL == *argv)
	{
		puts("\nFork: Allocation failed");
		return;
	}

	puts("(Fork) Enter a shell command: ");

	if (!scanf("%99s", *argv)) 
	{
		puts("\nFork: read input failed");
		return;
	}

	pid = fork();
	if (pid == 0)
	{
		execvp(*argv, argv);
		exit(0);
	}
	else if (pid < 0)
	{
		puts("\nFork: Can't fork, error occurred");
		exit(EXIT_FAILURE);
	}
	else
	{
		if (-1 == wait(NULL))
		{
			puts("\nFork: Parent wait function, error occurred");
		}
	}

	free(*argv);
}	


static void SystemImp(void)
{
	char *cmd = NULL;
	int status = 0;
	
	cmd = (char *)calloc(40, sizeof(char));
	if (NULL == cmd)
	{
		puts("System: Allocation failed");
		return;
	}
	
	puts("\n(System) Enter a shell command: ");

	status = scanf("%99s", cmd);
	if (!status) 
	{
		puts("System: read input failed");
		return;
	}

	if (0 < system(cmd))
	{
		puts("System: process could not be created");
		return;
	}

	free(cmd);
}