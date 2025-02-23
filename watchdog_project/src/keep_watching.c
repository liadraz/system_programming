
/*******************************************************************************
					- KEEP_WATCHING -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Implementation - KeepWatching
*	AUTHOR 		Liad Raz
*	REVIEWER		Ronen
*	DATE			14 Feb 2022
*
*******************************************************************************/

/* Used for setenv */
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>		/* printf, fflush */
#include <stdatomic.h>		/* atomic_size_t, atomic_fetch_add, atomic_store */
#include <stdlib.h>		/* getenv, atoi, malloc, free */
#include <unistd.h>		/* execvp, fork, getpid */
#include <sys/types.h>		/* pid_t */
#include <signal.h>		/* kill, sigaction, SIGUSR1-2, 
					sigset_t, sigemptyset, sigaddset, pthread_sigmask
					sigwait */
#include <time.h>			/* time_t  */

#include "keep_watching.h"
#include "utilities.h"		/* UNUSED */
#include "scheduler.h"		/* SchedCreate, SchedAdd, SchedRun, SchedPause, SchedDestroy */


#define RESET			0
#define I_THINK_YOU_DIED	5
#define MAX_DIGITS		12

typedef enum sched_status {RUNNING, PAUSE, WAIT, DNR} sched_status_ty;


typedef struct package_tasks
{
	pid_t m_to_watch;
	volatile atomic_size_t m_counter;	/* counter which checks if process is running */
	scheduler_ty *m_scheduler;
	volatile atomic_int m_sched_status;
	sigset_t m_sig_mask;
} pkg_tasks_ty;

/* Create global struct */
static pkg_tasks_ty for_tasks_g = {0};


/* Scheduler Tasks */
static int SendSigToOtherWatchDogT(void *param_);
static int CheckSigReceivedT(void *param_);

/* Revive the dead process */
static int ResuscitateProcessIMP(void *param_);

/* Auxilary Functions */
static int InitSignalHandlersIMP(void);
static void InitGlobalStructIMP(void *param, scheduler_ty *scheduler);

static pid_t ConvertEnvSTRtoIntIMP(void);
static char *UpdateParentEnvVarIMP(pid_t pid_);
static int BlockProcSignalMaskIMP(sigset_t *m_sig_mask_);
static int UNBlockProcSignalMaskIMP(sigset_t *m_sig_mask_);


/*******************************************************************************
****************************** KeepWatching ***********************************/
void *KeepWatching(void *process_info_)
{
	scheduler_ty *scheduler = NULL;
	sched_id_ty task_ret = {0};
	time_t task_interval = 2;

	/* Create scheduler */
	scheduler = SchedCreate();
	if (scheduler == NULL)
	{
		return NULL;
	}

	/* Initiate global struct */
	InitGlobalStructIMP(process_info_, scheduler);
	
	/* Initiate Signal Handlers */
	if (0 != InitSignalHandlersIMP())
	{
		/* TODO: atexit routine 
		free process_info_
		free scheduler */
		return NULL;
	}

	if (0 != UNBlockProcSignalMaskIMP(&(for_tasks_g.m_sig_mask)))
	{
		return NULL;
	}


	/* Add Tasks to the scheduler */
	task_ret = SchedAdd(scheduler, SendSigToOtherWatchDogT, &for_tasks_g, task_interval);
	if (UIDIsBad(task_ret))
	{
		/* TODO: atexit routine */
		return NULL;
	}

	task_ret = SchedAdd(scheduler, CheckSigReceivedT, &for_tasks_g, task_interval);
	if (UIDIsBad(task_ret))
	{
		/* TODO: atexit routine */
		return NULL;
	}

	printf("keep_watching.c:\n\tmy_pid\t%d, Env is\t%s\n", getpid(), getenv("WD_PID"));
      fflush(stdout);


	while (DNR != atomic_load(&for_tasks_g.m_sched_status))
	{
		SchedRun(scheduler);

		/* In case, scheduler stopped because process died */
 		if (PAUSE == atomic_load(&for_tasks_g.m_sched_status))
		{
			if (-1 == ResuscitateProcessIMP(process_info_))
			{
				/* TODO: atexit routine */
 				return NULL;
			}
 		}
	}

	SchedDestroy(scheduler);
	
	return NULL;
}

/*******************************************************************************
***************************** Tasks Functions *********************************/
/* Send signal to the sigHandler in order to check if other watchdog is running */
static int SendSigToOtherWatchDogT(void *param_)
{
	/* Increment global counter */
	atomic_fetch_add(&(for_tasks_g.m_counter), 1);

	/* Send signal to the SigHandler of the other watchdog */
	if (0 != kill(for_tasks_g.m_to_watch , SIGUSR1))
	{
		return 1;
	}

	UNUSED(param_);
	return 0;
}


/* Check if other watchdog still running */
static int CheckSigReceivedT(void *param_)
{
	/* In case, user invoked DNR func => STOP the scheduler */
	if (atomic_load(&for_tasks_g.m_sched_status) == DNR)
	{
		puts("Task DNR: Stop Scheduler");

		if (0 != kill(for_tasks_g.m_to_watch , SIGUSR2))
		{
			return 1;
		}

		/* To avoid Ping-Pong block the SIGUSR signal handlers */
		if (0 != UNBlockProcSignalMaskIMP(&(for_tasks_g.m_sig_mask)))
		{
			return 1;
		}

		/* Stop the running scheduler */
		SchedPause(for_tasks_g.m_scheduler);
	}

	/* In case, counter gets above a certain number => Pause the process */
	if (atomic_load(&for_tasks_g.m_counter) > I_THINK_YOU_DIED)
	{
		printf("I_THINK_YOU_DIED");

		/* Stop the running scheduler */
		SchedPause(for_tasks_g.m_scheduler);

		/* Update status variable */
		atomic_store(&(for_tasks_g.m_sched_status), PAUSE);
	}

	UNUSED(param_);
	return 0;
 }


/*******************************************************************************
****************************** Signal Handlers ********************************/
/* SIGUSR1 - SigHandler will reset the global counter indicating that signal received */
void ReceivedNReset(int signum_, siginfo_t *siginfo_, void *param_)
{
	atomic_store(&(for_tasks_g.m_counter), 0);

	printf("\nSIGUSR1 => Process pid\t%d\n", getpid());
      fflush(stdout);

	printf("Counter value:\t%lu\n", atomic_load(&for_tasks_g.m_counter));
      fflush(stdout);

	UNUSED(signum_);
	UNUSED(siginfo_);
	UNUSED(param_);
}


/* SIGUSR2 - SigHandler change the scheduler status causing it to stop */
void TerminateProcesses(int signum_, siginfo_t *siginfo_, void *param_)
{
	atomic_store(&(for_tasks_g.m_sched_status), DNR);

	UNUSED(signum_);
	UNUSED(siginfo_);
	UNUSED(param_);
}


/*******************************************************************************
**************************** Auxilary Functions *******************************/
/* TODO: Finish Revive func and its scheme */
static int ResuscitateProcessIMP(void *param_)
{
	pid_t child_pid = 0;
	pid_t env_wd_pid = 0;
	sigset_t m_sig_mask = {0};

	/* Make sure the other process is dead */
	if (0 != kill(((keep_watching_ty *)param_)->m_to_watch, SIGTERM))
	{
		return -1;
	}

	/* Convert WD_PID from string to integer */
	env_wd_pid = ConvertEnvSTRtoIntIMP();

	/* Create a signal mask and block both SIGUSR1 and SIGUSR2 signals */
	if (0 != BlockProcSignalMaskIMP(&for_tasks_g.m_sig_mask))
	{
		return -1;
	}

	/* fork the process */
	child_pid = fork();

	/* Handle Child Process */
	if (0 == child_pid)
	{
		/* In case client process died, 
		At the first time environment might be equal to NULL */
		if (getpid() == env_wd_pid) 
		{
			/* invoke execv with argv[0] the run_watchdog.c */
			execv(((keep_watching_ty *)param_)->m_argv[0], ((keep_watching_ty *)param_)->m_argv);
		}
		/* In case watchdog process died */
		else
		{
			/* invoke execv with argv[1] the keep_watching_main.c */
			execv(((keep_watching_ty *)param_)->m_argv[1], ((keep_watching_ty *)param_)->m_argv);
		}
	}
	else if (-1 == child_pid)
	{
		return -1;
	}

	/* Handle Parent Process => 
	In case WD Process died, update the environment variable with the new child pid
	In Case Client Died, Update the param_ with the new child pid */
	if (NULL == UpdateParentEnvVarIMP(child_pid))
	{
		return -1;
	}

	/* Update the parent process scheduler status to tell it to start running */
	atomic_store(&(for_tasks_g.m_sched_status), RUNNING);

	return 0;
}

static int InitSignalHandlersIMP(void)
{
	struct sigaction saHandler1 = {NULL};
	struct sigaction saHandler2 = {NULL};

	/* Initiate sigaction structs with SA_SIGINFO flag and their relevant functions */
	saHandler1.sa_flags = SA_SIGINFO;
	saHandler1.sa_sigaction = ReceivedNReset;

	/* set a signal disposition of SIGUSR1 with ReceivedNReset function */
	if (-1 == sigaction(SIGUSR1, &saHandler1, NULL))
	{
		return 1;
	}

	saHandler2.sa_flags = SA_SIGINFO;
	saHandler2.sa_sigaction = TerminateProcesses;

	/* set a signal disposition of SIGUSR2 with ReceivedNReset function */
	if (-1 == sigaction(SIGUSR2, &saHandler2, NULL))
	{
		return 1;
	}

	return 0;
}

static void InitGlobalStructIMP(void *param_, scheduler_ty *sched_)
{
	for_tasks_g.m_to_watch = ((keep_watching_ty *)param_)->m_to_watch;
	atomic_store(&(for_tasks_g.m_counter), RESET);
	for_tasks_g.m_scheduler = sched_;
	atomic_store(&(for_tasks_g.m_sched_status), RUNNING);
}


static pid_t ConvertEnvSTRtoIntIMP(void)
{
	pid_t pid = 0;
	char *str_pid = NULL;

	str_pid = (char *)malloc(sizeof(MAX_DIGITS));
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


static char *UpdateParentEnvVarIMP(pid_t pid_)
{
	char *converted_pid = NULL;

      converted_pid = (char *)malloc(sizeof(pid_t) * sizeof(char));
      if (NULL == converted_pid)
      {
            return NULL;
      }

	/* get the current pid value and convert it to string */
      if (0 >= sprintf(converted_pid, "%d", pid_))
      {
            free(converted_pid);
		return NULL;
      }

	/* update the environment variable */
	if (0 != setenv("WD_PID", converted_pid, 1))
	{
		return NULL;
	}

	free(converted_pid);

	return converted_pid;
}



static int BlockProcSignalMaskIMP(sigset_t *m_sig_mask_)
{
	/* Create a Mask which will inherent to the child process, 
	block the SIGUSR1 and SIGUSR2 */
	sigemptyset(m_sig_mask_);
	sigaddset(m_sig_mask_, SIGUSR1);
	sigaddset(m_sig_mask_, SIGUSR2);

	if (0 != pthread_sigmask(SIG_BLOCK, m_sig_mask_, NULL))
	{
		return -1;
	}

	return 0;
}


static int UNBlockProcSignalMaskIMP(sigset_t *m_sig_mask_)
{
	/* Create a Mask which will inherent to the child process, 
	block the SIGUSR1 and SIGUSR2 */
	sigemptyset(m_sig_mask_);
	sigaddset(m_sig_mask_, SIGUSR1);
	sigaddset(m_sig_mask_, SIGUSR2);

	if (0 != pthread_sigmask(SIG_UNBLOCK, m_sig_mask_, NULL))
	{
		return -1;
	}

	return 0;
}



