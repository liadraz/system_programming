
/*******************************************************************************
					- KEEP_WATCHING -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		API - Shared Library Files KeepWatching Functions
*	AUTHOR 		Liad Raz
*	REVIEWER		Ronen
*	DATE			14 Feb 2022 <3
*
*******************************************************************************/

#ifndef __KEEP_WATCHING_H__
#define __KEEP_WATCHING_H__

#include <signal.h>	/* siginfo_t */


/* Keeps the other process pid, and path to both APIs */
typedef struct keep_watching
{
	pid_t 	m_to_watch;
	int 		m_argc;
	char 		**m_argv;
} keep_watching_ty;


/*******************************************************************************
* DESCRIPTION	Checks if watchdog process is alive and still running.
*
*******************************************************************************/
void *KeepWatching(void *param);


/*******************************************************************************
* DESCRIPTION	SignalHandler - SIGUSR1
*			Indicate a signal recieved and the current process is alive.
*******************************************************************************/
void ReceivedNReset(int signum, siginfo_t *siginfo, void *param);


/*******************************************************************************
* DESCRIPTION	SignalHandler - SIGUSR2
			Pause the watchdog guardening.
*
*******************************************************************************/
void TerminateProcesses(int signum, siginfo_t *siginfo, void *param);


#endif /* __KEEP_WATCHING_H__ */

