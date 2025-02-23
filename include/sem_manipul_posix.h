/*******************************************************************************
					- SEM_MANIPUL_POSIX -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		API -  Semaphore Manipulation using POSIX API
*	AUTHOR 		Liad Raz
*	REVIEWER		Yanal
*	DATE			06 Feb 2022
*	FILES			sem_manipul_posix.c sem_manipul_posix_test.c sem_manipul_posix.h
* 
*******************************************************************************/

#ifndef __SEM_MANIPUL_POSIX_H__
#define __SEM_MANIPUL_POSIX_H__


/*******************************************************************************
* DESCRIPTION	Creates a Sempahore of a given name, and allow manipulation of
			it from the command line.
*
*******************************************************************************/
void SemManipulation(char *sem_name);


#endif /* __SEM_MANIPUL_POSIX_H__ */

