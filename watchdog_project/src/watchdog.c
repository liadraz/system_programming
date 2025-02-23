/* TODO: 
1. Atexit complete it. Check which functions needs to restore resources
2. second argument pass it with fixed defined macro.
3. Think about utils API.
*/
/*******************************************************************************
					- WATCHDOG -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Implementation
*	AUTHOR 			Liad Raz
*	REVIEWER		Ronen
*	DATE			14 Feb 2022
*
*	CPYCompile		gd -C -pthread src/watchdog.c -I ../ -I ./include/
*******************************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>			/* printf, puts, fflush */
#include <stdlib.h>			/* getenv, atoi, malloc, free */
#include <unistd.h>			/* execv, fork, getpid */
#include <sys/types.h>		/* pid_t */
#include <pthread.h>		/* pthread_t, pthread_create, pthread_detach */
#include <signal.h>			/* kill */

#include <assert.h>			/* assert */
#include <unistd.h>			/* write */

#include "utilities.h"
#include "watchdog.h"
#include "keep_watching.h"


#define MAX_DIGITS	12


static pid_t ConvertEnvSTRtoIntIMP(void);
static int BlockProcSignalMaskIMP(sigset_t *sig_mask_);


/*******************************************************************************
******************************** KeepMeAlive **********************************/
int KeepMeAlive(int argc, char *argv[])
{
	keep_watching_ty *process_info = NULL;
	pthread_t thread_id = {0};
	sigset_t sig_mask = {0};
	pid_t ret_pid = 0;

	assert(argv != NULL);

	process_info = (keep_watching_ty *)malloc(sizeof(keep_watching_ty));
	if (NULL == process_info)
	{
		/* TODO: atexit routine */
		return 1;
	}

	/* Create a signal mask and block both SIGUSR1 and SIGUSR2 signals */
	if (0 != BlockProcSignalMaskIMP(&sig_mask))
	{
		free(process_info);
		return 1;
	}

	/* Check If watchdog does not exist */
	if (getenv("WD_PID") == NULL)
	{
		puts("watchdog.c:\n\tFirst Initiation");

		/* Create a new process which will be the watchdog process */
		ret_pid = fork();

		/* Handle the child process --> execute watchdog process */
		if (0 == ret_pid)
		{
			printf("\nForked: Child pid\t%d\n", ret_pid);
			fflush(stdout);

			/* invoke execv with keep_watching file */
			execv(argv[1], argv);
		}
		/* In case, of a failure */
		else if (-1 == ret_pid)
		{
			/* TODO: atexit routine */
			return 1;
		}
		/* Handle the Parent process --> save in parent the child born pid */
		process_info->m_to_watch = ret_pid;

		fprintf(stdout , "\nForked: Parent pid\t%d", ret_pid);
	}
	else
	{
		puts("watchdog.c:\n\tWD has initiated at least once ");

		/* In case WD process exists convert env WD_PID from string to integer */
		process_info->m_to_watch = ConvertEnvSTRtoIntIMP();
	}

	process_info->m_argc = argc;
	process_info->m_argv = argv;

	/* Handle the parent watch dog --> create thread watchdog */
	if (0 != pthread_create(&thread_id, NULL, KeepWatching, process_info))
	{
		/* TODO: atexit routine */
		free(process_info);
		return 1;
	}

	/* use pthread detach to wait for the thread to finish its job */
	if (0 != pthread_detach(thread_id))
	{
		/* TODO: atexit routine */
		free(process_info);
		return 1;
	}

	free(process_info);

	return 0;
}

/*******************************************************************************
******************************** DoNotResuscitate *****************************/
int DoNotResuscitate(void)
{
	/* Send Signal to myself in order to stop my own watchdog routine.
	 The sigHandler will send a signal to the other watchdog in order to stop it
	 as well */
	if (0 != kill(getpid() , SIGUSR2))
	{
		return 1;
	}

	/* Invoke atexit to free all resources */

	return 0;
}


/*******************************************************************************
**************************** Auxilary Functions *******************************/
static pid_t ConvertEnvSTRtoIntIMP(void)
{
	pid_t pid = 0;
	char *str_pid = NULL;

	str_pid = (char *)malloc(MAX_DIGITS);
	if (NULL == str_pid)
	{
		return -1;
	}

	str_pid = getenv("WD_PID");

	if (*str_pid == '\0')
	{
		return 0;
	}

	pid = atoi(str_pid);

	return pid;
}

static int BlockProcSignalMaskIMP(sigset_t *sig_mask_)
{
	/* Create a Mask which will inherent to the child process, 
	block the SIGUSR1 and SIGUSR2 */
	sigemptyset(sig_mask_);
	sigaddset(sig_mask_, SIGUSR1);
	sigaddset(sig_mask_, SIGUSR2);

	if (0 != pthread_sigmask(SIG_BLOCK, sig_mask_, NULL))
	{
		return -1;
	}

	return 0;
}

 
/*
MAP FILES => 
---------------
RUN
---------------
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH

gd test/run_watchdog.c src/watchdog.c -pthread -I ../../ -I ./include/ -lwatchdog_common -L. libwatchdog.a -o run_watchdog.out
	run_watchdog.out

gd -pthread ./test/keep_watching_main.c -I ./include/ -lwatchdog_common -L. -o keep_watching.out
	keep_watching_main.c
	==>
	keep_watching_main.out

---------------execv
watchdog.h

---------------
Dynamic Library
---------------
Create the shared library ==>
gd -fpic -shared -o libwatchdog_common.so ./shared_objs/dlinked_list_so.o ./shared_objs/uid_so.o ./shared_objs/soreted_list_so.o ./shared_objs/pqueue_so.o ./shared_objs/scheduler_so.o keep_watching_so.o


Create object files =>
gd -fpic -pthread -c -I ../../ -I ./include/ -I ../../ds/include/ src/keep_watching.c -o keep_watching_so.o
	keep_watching.c
	keep_watching.h	keep_watching(), signalhandler()


gd -fpic -c -I ../ -I ./include/ src/name.c -o name_so.o
	scheduler.o
	pqueue.o
	sorted_list.o
	dlinked_list.o
	uid.o

*/
