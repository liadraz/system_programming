/* P2 process
*******************************************************************************/

#include <stdio.h>			/* printf */
#include <string.h>			/* strlen */
#include <unistd.h>			/* write, read, close, sleep */
#include <sys/types.h>		/* O_WRONLY, O_RDONLY */
#include <sys/stat.h>		/* mkfifo */
#include <fcntl.h>		    /* open */

#define MAX_BUFF	1024

void FIFOPong(void);


int main(void)
{
	FIFOPong();

	return 0;
}

/*******************************************************************************
Q.3		IPC using a FIFO (named pipe)
        NOTE    For simplicity there are no checks for sysCalls return values.
*******************************************************************************/
void FIFOPong(void)
{
    int fd = 0;
	const char *PATH = "/home/liad/git/system_programming/ipc/fifo.txt";
    
    char *pong_msg = "P2 Pong";
	char read_buff[MAX_BUFF] = {0};
	
	read_buff[MAX_BUFF] = '\0';

	/* Create the FIFO (named pipe) */
    mkfifo(PATH, 0666);

	while (1)
	{
		/* Receive PING From P1 
		*************************/

		/* Open FIFO to read only */
		fd = open(PATH, O_RDONLY);
		
		/* Read "Ping" from the FIFO */
		read(fd, read_buff, sizeof(read_buff));
		/* Printf the "Ping" Msg */
		printf("P2 Received: %s\n", read_buff);
		close(fd);
		

		/* Send PONG To P1 
		*************************/

		/* Open FIFO to write only */
		fd = open(PATH, O_WRONLY);

		/* Write "Pong" in the FIFO and close the fd */
		write(fd, pong_msg, strlen(pong_msg) + 1);
		close(fd);

		sleep(2);
	}
}