
/*******************************************************************************
					- KEEP_WATCHING -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		Main - Invoke KeepWatching Functions 
                        for the watchdog process
*	AUTHOR 		Liad Raz
*	REVIEWER		Ronen
*	DATE			14 Feb 2022
*
*******************************************************************************/

/* Used for getenv */
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>      /* sprintf */
#include <stdlib.h>     /* setenv, malloc, free*/
#include <unistd.h>     /* getpid */
#include <sys/types.h>  /* pid_t */

#include "keep_watching.h"

#define MAX_DIGITS	12

static int SetPidToEnvVarIMP(void);


int main(int argc, char *argv[])
{
      keep_watching_ty *process_info = NULL;

      /* assert(argc, argv) */

      if (0 != SetPidToEnvVarIMP())
      {
            /* TODO: atexit routine */
            return 1;
      }
      
      /* Create the process_info struct */
      process_info = (keep_watching_ty *)malloc(sizeof(keep_watching_ty));
      if (NULL == process_info)
      {
            /* TODO: atexit routine */
            return 1;
      }

      /* Initiate the process_info struct */
      process_info->m_to_watch = getppid();
      process_info->m_argc = argc;
      process_info->m_argv = argv;

      printf("keep_watching_main:\n\tI am the child WD\nmy_pid\t%d, my_ppid\t%d\n", getpid(), getppid());
      printf("Env is\t%s\n\n", getenv("WD_PID"));
      fflush(stdout);

      if (0 != KeepWatching(process_info))
      {
            return 1;
      }

      free(process_info);
      return 0;
}


/*******************************************************************************
**************************** Auxilary Functions *******************************/

static int SetPidToEnvVarIMP(void)
{
      char *pid_myself = NULL;

      pid_myself = (char *)malloc(MAX_DIGITS);
      if (NULL == pid_myself)
      {
            return 1;
      }

      /* get the current pid value and convert it to string */
      if (0 >= sprintf(pid_myself, "%d", getpid()))
      {
            free(pid_myself);
		return 1;
      }

      /* set an environment variable which will hold the current pid */
      if (0 != setenv("WD_PID", pid_myself, 1))
      {
		return 1;
      }

      free(pid_myself);

      return 0;
}