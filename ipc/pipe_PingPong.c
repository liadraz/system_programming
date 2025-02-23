/*******************************************************************************
					Pipe - Ping Pong
********************************************************************************
*
*	DESCRIPTION		Implement Ping Pong between two Related processes by using
*					a PIPE.
*	AUTHOR 			Liad Raz
*	DATE			05 Apr 2022
*
*******************************************************************************/

#include <stdio.h>			/* puts, FILE, fdopen, feof, ferror, 
							fgets, fputs, stdout, fprintf, fflush */
#include <string.h>			/* strlen */
#include <unistd.h>			/* pipe, fork */
#include <sys/types.h>		/* pid_t */


/* Funcs Decelerations */
/******************************************************************************/

#define BLOCK	1024

/* Pipe */
typedef enum io {READ_END, WRITE_END} io_ty;

void Pipe(void);
void PipeWriter(const char* msg_, FILE* stream_);
void PipeReader(FILE* stream_);


int main(void)
{
	Pipe();

	return 0;
}

/*******************************************************************************
Q.2		IPC using a Pipe
*******************************************************************************/

void Pipe(void)
{
	int ping_fds[2] = {0};
	int pong_fds[2] = {0};
	pid_t ret_pid = 0;

	/* Call to pipe 
		reading fd stored in position 0 
		writing fd stored in position 1 
	NOTE	fds are inherited to the child process 
			Pipe automatically synchronizes the two process */
	if (-1 == pipe(ping_fds))
	{
		puts("Error in creating a Pipe");
		return;
	}

	if (-1 == pipe(pong_fds))
	{
		puts("Error in creating a Pipe");
		return;
	}

	ret_pid = fork();
	
	/* Handle the Parent process, Sends Ping --> To child --> Then Receives Pong */
	if (ret_pid > 0)
	{
		FILE *stream = {0};
		char *ping = "P1, Ping";

		/* Close the unwanted copy of read file discriptor */
		close(ping_fds[READ_END]);	/* parent only writes ping */
		/* First Write a "Ping" message to stream and send it to the Child
		/* by converting the write file discriptor to a FILE object, and writing to it */
		stream = (FILE *)fdopen(ping_fds[WRITE_END], "w");
		PipeWriter(ping, stream);
		close(ping_fds[WRITE_END]);

		
		/* Close the unwanted copy of write file discriptor */
		close(pong_fds[WRITE_END]); /* parent only reads pong */
		/* Wait to get Pong and read it from stream */
		stream = (FILE *)fdopen(pong_fds[READ_END], "r");
		PipeReader(stream);
		close(pong_fds[READ_END]);
	}
	/* Handle the child process, Receives Ping --> From parent --> Then Sends Pong */
	else if (0 == ret_pid)
	{
		FILE *stream = {0};
		char *pong = "P2, Pong";
		
		/* Close the unwanted copy of write file discriptor */
		close(ping_fds[WRITE_END]); /* child only reads ping */
		/* Wait to Read from stream "Ping" which was sent by Parent */
		/* Convert the read file discriptor to a FILE object, and read from it */
		stream = (FILE *)fdopen(ping_fds[READ_END], "r");
		PipeReader(stream);
		close(ping_fds[READ_END]);

	/* Close the unwanted copy of read file discriptor */
		close(pong_fds[READ_END]); /* child only writes pong */
		/* Writes Pong massage and write it on stream */
		stream = (FILE *)fdopen(pong_fds[WRITE_END], "w");
		PipeWriter(pong, stream);
		close(pong_fds[WRITE_END]);
	}
	/* In case of a failure */
	else { return; }

}

void PipeWriter(const char* msg_, FILE* stream_)
{
	/* Write the message to the stream and send it off immediately (flush it) */
	fprintf(stream_, "%s\n" ,msg_);
	fflush(stream_);
}

void PipeReader(FILE* stream_)
{
	char buffer[BLOCK] = {0};

	/* read until reaching eof / error / invalid string */
	while (
		!feof(stream_) 
		&& !ferror(stream_) 
		/* store stream data in buffer. A terminating null byte is stored at the last character in buffer */
		&& fgets(buffer, BLOCK, stream_) != NULL)	
	{
		fputs(buffer, stdout);
	}
}