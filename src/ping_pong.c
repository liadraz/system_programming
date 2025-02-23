
/*******************************************************************************
					- PING_PONG -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Implementation
*	AUTHOR 		Liad Raz
*	REVIEWER		Ira
*	DATE			01 Feb 2022
*
*	CPYCOMPILE		gc -D_POSIX_C_SOURCE=199309L -I ../ src/ping_pong.c
*******************************************************************************/

#include <stdio.h>			/* printf, puts, stdout, NULL */
#include <unistd.h>			/* execv, fork, sleep, write */
#include <sys/types.h>			/*  pid_t */
#include <sys/wait.h>			/*  wait */
#include <signal.h>			/* kill, SIGUSR1-2, sig_atomic_t, sigaction */
#include <string.h>			/* memset, strlen */
#include <stdlib.h>			/* exit */

#include "utilities.h"			/* UNUSED */

#define RUN		1


void PingPongFork(void);
void MsgPingPongChild(int signum_, siginfo_t *sginfo_, void *param_);
void MsgPingPongParent(int signum_, siginfo_t *sginfo_, void *param_);

void PingPongForkNExe(void);
void PingParentForkNExe(int signum_, siginfo_t *sginfo_, void *param_);

/* Define a sig_atomic_t variable in which is guaranteed that 
assignments of this type are performed in a single instruction 
and therefore cannot be interrupted midway */
sig_atomic_t sig_fork_count = 0;
sig_atomic_t sig_fork_exe_count = 0;


/*******************************************************************************
******************************* PING-PONG *************************************/

int main(void)
{
	puts("\n\t--- PING_PONG ---\n");

	puts("\n\tFork\n");
	PingPongFork();

	puts("\n\tFork and Execute\n");
	PingPongForkNExe();

	return 0;
}



/*******************************************************************************
******************************* Ping-Pong Fork ********************************/

void PingPongFork(void)
{
	pid_t child_pid_ = 0;

	/* Create sigaction structs for both child and parent */
	struct sigaction saParent = {NULL};
	struct sigaction saChild = {NULL};

	/* Assign function to the signal-handler-function member in the sa struct */
	saParent.sa_flags = SA_SIGINFO;
	saParent.sa_sigaction = MsgPingPongParent;

	/* set a signal disposition of the MsgPingPongParent provided function */
	if (-1 == sigaction(SIGUSR1, &saParent, NULL))
	{
		puts("sigaction: Error occurred");
		return;
	}

	/* Create a child process */
	child_pid_ = fork();

	if (-1 == child_pid_)
	{
		puts("fork: Error occurred");
		return;

	}
	/* I am the child process */
	else if (0 == child_pid_)
	{
		saChild.sa_flags = SA_SIGINFO;
		saChild.sa_sigaction = MsgPingPongChild;

		/* set a signal disposition of the MsgPingPongChild provided function */
		if (-1 == sigaction(SIGUSR2, &saChild, NULL))
		{
			puts("sigaction: Error occurred");
			return;
		}

		/* Keep the child alive until killing it by his parent */
		while (RUN)
		{
			;
		}
	}
	/* I am the parent process */
	else
	{
		/* let child process build its signal action struct */
		sleep(1);
		
		/* Invoke child signal handler */
		kill(child_pid_, SIGUSR2);

		while(sig_fork_count < 12)
		{
			/* stop parent process until getting a signal */
			pause();
		}

		/* Terminate the child process */
		kill(child_pid_, SIGTERM);

		if (-1 == wait(NULL))
		{
			puts("\nERROR occurred, clearing child process");
		}
	}

	printf("SIGNAL counts =>\t%d\n", sig_fork_count);
}

/*******************************************************************************
**************************** Signal Handlers **********************************/

void MsgPingPongChild(int signum_, siginfo_t *sginfo_, void *param_)
{
	char *str = "I am a Child, You sent me SIGUSR2 \tPING\n";
	++sig_fork_count;
	
	if (-1 == write(1, str, strlen(str)))
	{
		return;
	}

	kill(sginfo_->si_pid, SIGUSR1);

	UNUSED(signum_);
	UNUSED(param_);
}


void MsgPingPongParent(int signum_, siginfo_t *sginfo_, void *param_)
{
	char *str = "I am a Parent, You sent me SIGUSR1 \tPONG\n";
	++sig_fork_count;
	
	if (-1 == write(1, str, strlen(str)))
	{
		return;
	}

	kill(sginfo_->si_pid, SIGUSR2);

	UNUSED(signum_);
	UNUSED(param_);
}



/*******************************************************************************
*************************** Ping-Pong Fork & Exe ******************************/
void PingPongForkNExe(void)
{
	pid_t child_pid_ = 0;
	char *argv[] = {"exe.out", NULL};

	/* Create sigaction structs for both child and parent */
	struct sigaction saParent = {NULL};

	/* Assign function to the signal-handler-function member in the sa struct */
	saParent.sa_flags = SA_SIGINFO;
	saParent.sa_sigaction = PingParentForkNExe;

	/* set a signal disposition of the PingParentForkNExe provided function */
	if (-1 == sigaction(SIGUSR1, &saParent, NULL))
	{
		puts("sigaction: Error occurred");
		return;
	}

	/* Create a child process */
	child_pid_ = fork();

	if (-1 == child_pid_)
	{
		puts("fork: Error occurred");
		return;
	}
	/* I am the child process */
	else if (0 == child_pid_)
	{
		execv(argv[0], argv);
	}

	/* let child process build its signal action struct */
	sleep(1);
	
	/* Invoke child signal handler */
	kill(child_pid_, SIGUSR2);

	while(sig_fork_exe_count < 10)
	{
		/* stop parent process until getting a signal */
		pause();
	}

	/* Terminate the child process */
	kill(child_pid_, SIGTERM);

	if (-1 == wait(NULL))
	{
		puts("\nERROR occurred, clearing child process");
	}
}

void PingParentForkNExe(int signum_, siginfo_t *sginfo_, void *param_)
{
	char *str = "I am a Parent, You sent me SIGUSR1 \tPONG\n";
	++sig_fork_exe_count;
	
	if (-1 == write(1, str, strlen(str)))
	{
		return;
	}

	kill(sginfo_->si_pid, SIGUSR2);

	UNUSED(signum_);
	UNUSED(param_);
}