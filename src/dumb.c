
/*******************************************************************************
*************************** - DUMB & DUMBER - **********************************
********************************************************************************
*
*	DESCRIPTION		dumb.c
*	AUTHOR 			Liad Raz
*	DATE			08 Dec 2021
*
*******************************************************************************/

#include <stdio.h>		/* fprint, printf, fopen */
#include <unistd.h>		/* sleep */

int main(int argc, char **argv, char **envp)
{
	int i = 7;
	int j = 12;
	int k = 15;
	int *p = &j;
	
	FILE *fp;
	
	if (0 == (fp = fopen("stoopid", "w")))
	{
		printf("well, that did\'t work!\n");
		return -1;
	}
	
	fprintf(fp, "%p\n", (void *)p);
	
	if(fclose(fp))
	{
		printf("oh, well.");
		return -1;
	}

	sleep(300);
	
	(void)i;
	(void)k;
	(void)argc;
	(void)argv;
	(void)envp;
	
	return 0;
}

/* echo 0 | sudo tee /proc/sys/kernel/randomize_va_space */
