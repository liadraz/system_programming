/*******************************************************************************
					- WATCHDOG -
*************************** SYSTEM PROGRAMMING *********************************
*
*	DESCRIPTION		WatchDog - API
*	AUTHOR 		Liad Raz
*	REVIEWER		Ronen
*	DATE			14 Feb 2022
*	FILES			watchdog.c watchdog.h
* 
*******************************************************************************/

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__


/*******************************************************************************
* DESCRIPTION	Creates a process watchDog that will guard your program in case
			of unintended termination.
* RETURN	 	status => 0 Success; 1 Failure
* IMPORTANT		User should provide the path to watchdog executable.
			When running second argument (argv[1]) 
			should be the watchdog executable.
*
*******************************************************************************/
int KeepMeAlive(int argc, char *argv[]);


/*******************************************************************************
* DESCRIPTION	Tell the watchdog process to stop its guardening.
* RETURN	 	status => 0 Success; 1 Failure 
*
*******************************************************************************/
int DoNotResuscitate(void);


#endif /* __WATCHDOG_H__ */

