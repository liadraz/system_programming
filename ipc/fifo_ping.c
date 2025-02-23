/*******************************************************************************
					FIFO - Ping Pong
********************************************************************************
*
*	DESCRIPTION		Implement Ping Pong between two UNRelated processes by using
*					FIFO (named pipes).
*	AUTHOR 			Liad Raz
*	DATE			09 Apr 2022
*   
*   FILES           fifo_ping.c fifo_pong.c
*******************************************************************************/

#include <stdio.h>			/* printf */
#include <string.h>			/* strlen */
#include <unistd.h>			/* write, read, close, sleep */
#include <sys/types.h>		/* O_WRONLY, O_RDONLY */
#include <sys/stat.h>		/* mkfifo */
#include <fcntl.h>		    /* open */

#define MAX_BUFF	1024

void FIFOPing(void);


int main(void)
{
    /* P1 process */
    FIFOPing();

	return 0;
}


/*******************************************************************************
Q.3		IPC using a FIFO (named pipe)
        NOTE    For simplicity there are no checks for sysCalls return values.
*******************************************************************************/
void FIFOPing(void)
{
    int fd = 0;
    const char *PATH = "/home/liad/git/system_programming/ipc/fifo.txt";
    
    char *ping_msg = "P1 Ping";
    char read_buff[MAX_BUFF] = {0};
    
    read_buff[MAX_BUFF] = '\0';
    
    /* 
        Create the FIFO (named pipe) 
        syntax  mkfifo(PATH, permissions) 
    */
    mkfifo(PATH, 0666); 

    while (1)
    {
        /* Send PING To P2
		*************************/

        fd = open(PATH, O_WRONLY);
        /* write "Ping" to the FIFO and close the fd */
        write(fd, ping_msg, strlen(ping_msg) + 1);
        close(fd);

       /*  sleep(2); */


        /* Receive PONG From P2
		*************************/
        
        /* Open FIFO to read only */
        fd = open(PATH, O_RDONLY);

        /* Read "Pong" from the FIFO */
        read(fd, read_buff, sizeof(read_buff));

        /* Printf the "Pong" Msg */
        printf("P1 Received: %s\n", read_buff);
        close(fd);

    }
}

/* 
    A Fifo can have multiple readers or writers.
    Bytes from each writer are written atomically up to maximum size of PIPE_BUF (4KB on Linux).
*/
