
/*******************************************************************************
					- PING_PONG -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		Exercise (N.2) Fork and Execute - Impl the Child Process
*	AUTHOR 		Liad Raz
*	REVIEWER		Ira
*	DATE			01 Feb 2022
*
*	CPYCOMPILE		gc -D_POSIX_C_SOURCE=199309L src/ping_pong_exe.c -o exe.out
*******************************************************************************/

#include <stddef.h>                 /* NULL */
#include <stdio.h>			/* printf, puts, stdout */
#include <unistd.h>			/* write */
#include <signal.h>			/* kill, SIGUSR1-2, sig_atomic_t, sigaction */
#include <string.h>			/* strlen */

#define RUN		1

void ProcessChild(void);
void PongParentForkNExe(int signum_, siginfo_t *sginfo_, void *param_);


/*******************************************************************************
*************************** Ping-Pong Child Process ***************************/

int main(void)
{
      ProcessChild();
      
      return 0;
}

void ProcessChild(void)
{
      struct sigaction saChild = {NULL};

      saChild.sa_flags = SA_SIGINFO;
      saChild.sa_sigaction = PongParentForkNExe;

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


void PongParentForkNExe(int signum_, siginfo_t *sginfo_, void *param_)
{
	char *str = "I am a Child, You sent me SIGUSR2 \tPING\n";
	
	if (-1 == write(1, str, strlen(str)))
	{
		return;
	}

	kill(sginfo_->si_pid, SIGUSR1);

	(void)(signum_);
	(void)(param_);
}