/*******************************************************************************
					- SEM_MANIPULATION_SV -
***************************** DATA STRUCTURES **********************************
*
*	DESCRIPTION		API - Semaphore Manipulation using System V API
*	AUTHOR 		Liad Raz
*	REVIEWER		Yanal
*	DATE			06 Feb 2022
*	FILES			sem_manipulation_sv.c sem_manipulation_sv_test.c sem_manipulation_sv.h
* 
*******************************************************************************/

#ifndef __SEM_MANIPULATION_SV_H__
#define __SEM_MANIPULATION_SV_H__


/*******************************************************************************
* DESCRIPTION	Creates a Sempahore of a given name, and allow manipulation of
			it from the command line.
*
*******************************************************************************/
void SemManipulation(char *sem_name);


#endif /* __SEM_MANIPULATION_SV_H__ */

