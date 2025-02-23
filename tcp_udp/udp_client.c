
/*******************************************************************************
					UDP connection
********************************************************************************
*
*	DESCRIPTION		Client Side Impl UDP ping-pong Messages using 
                    UDP connection.
*	AUTHOR 			Liad Raz
*	DATE			10 Apr 2022
*   FILES           udp_client.c
                    udp_server.c
*******************************************************************************/

/*
    In UDP, Client sends a datagram.
    The Server, need not accept a connection, just waits for datagram to arrive.

    Datagrams upon arrival contain the address of the sender which the Server uses to send data to the correct Client.

    The entire process in steps:
    ----------------------------
    UDP Client :
        1. Create UDP socket.
        2. Send a message to the server.
        3. Wait until response from the Server is received.
        4. Process reply and go back to step 2, if necessary.
        5. Close socket descriptor and exit.

*/

#include <stdio.h>          /* printf */
#include <string.h>         /* strlen, memset */
#include <stdlib.h>         /* exit, EXIT_FAILURE */
#include <netinet/in.h>     /* sockaddr_in, */
#include <unistd.h>         /* close */
#include <sys/socket.h>     /* socket, AF_INET, SOCK_DGRAM, sendto, recvform */
#include <arpa/inet.h>      /* htons */

#define PORT    8080
#define MAXLINE 1024

int main (void)
{
    char buffer[MAXLINE] = {0};
    char *msg = "Ping";

    int sock_fd = 0;
    /* The basic struct for all syscalls and functions that deal with internet addresses. sockaddr_in compitable only with IPV4 addresses */
    struct sockaddr_in server_addr;

    socklen_t from_length = 0;
    int ret_num_bytes = 0;

/* 1. */
    /* Creating a socket file descriptor. param:    domain, type UDP, protocol 
    Use PF_INET in the call to socket(). and AD_INET in the struct */
    if ((sock_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        exit(EXIT_FAILURE);
    }

    /* Reset the struct */
    memset(&server_addr, 0, sizeof(server_addr));

    /* Filling server Information */
    server_addr.sin_family = AF_INET;   /* IPV4 */
    /* Convert the port number to newtwork byte order */
    server_addr.sin_port = htons(PORT);
    /* INADDR_ANY IP is automatically being selected */
    server_addr.sin_addr.s_addr = INADDR_ANY;

/* 2. */
    /* Send msg to the server */
    sendto(sock_fd, msg, strlen(msg), MSG_CONFIRM, (struct sockaddr*)&server_addr, sizeof(server_addr));

/* 3. */
    /* Receive msg from the server */
    ret_num_bytes = recvfrom(sock_fd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&server_addr, &from_length);

    buffer[ret_num_bytes] = '\0';

    printf("Server Sent: %s\n", buffer);

    printf("from_length %d\n", from_length);        /*  */
    printf("ret_num_bytes %d\n", ret_num_bytes);    /* pong length is 4 */

    /* Close socket descriptor */
    close(sock_fd);

    return 0;
}