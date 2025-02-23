
/*******************************************************************************
*************************** - DUMB & DUMBER - **********************************
********************************************************************************
*
*	DESCRIPTION		dumber.c
*	AUTHOR 			Liad Raz
*	DATE			08 Dec 2021
*
*******************************************************************************/

#include <stdio.h>		/* fscanf, printf, fopen */
#include <unistd.h>		/* sleep */

int main(int argc, char **argv, char **envp)
{
	int i = 8;
	int j = 16;
	int k = 32;
	int *p = &j;
	
	FILE *fp;
	
	if (0 == (fp = fopen("stoopid", "r")))
	{
		printf("well that didn\'t work!\n");
		return -1;
	}
	
	i = fscanf(fp, "%p\n", (void **)&p);
	
	if (fclose(fp))
	{
		printf("oh well.");
		return -1;
	}
	
	printf("p point to :%d\n", *p);
	
	(void)i;
	(void)k;
	(void)argc;
	(void)argv;
	(void)envp;
	
	return 0;
}

