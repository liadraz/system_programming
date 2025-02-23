
/*******************************************************************************
					UDP connection
********************************************************************************
*
*	DESCRIPTION		Server Side Impl UDP ping-pong Messages using 
                    UDP connection.
*	AUTHOR 			Liad Raz
*	DATE			10 Apr 2022
*   FILES           udp_client.c
                    udp_server.c
*******************************************************************************/

/*
    The entire process in steps:
    ----------------------------
    UDP Server :
        1. Create a UDP socket.
        2. Bind the socket to the server address.
        3. Wait until the datagram packet arrives from the client.
        4. Process the datagram packet and send a reply to the client.
        5. Go back to Step 3.
*/

#include <stdio.h>          /* printf */
#include <string.h>         /* strlen, memset */
#include <stdlib.h>         /* exit, EXIT_FAILURE */
#include <netinet/in.h>     /* sockaddr_in, */
#include <unistd.h>         /* close */
#include <sys/socket.h>     /* socket, PF_INET, AF_INET, SOCK_DGRAM, sendto, recvform, bind */
#include <sys/types.h>
#include <arpa/inet.h>      /* htons */


#define PORT    8080
#define MAXLINE 1024

int main (void)
{
    char buffer[MAXLINE] = {0};
    char *msg = "Pong";

    int sock_fd = 0;
    /* The basic struct for all syscalls and functions that deal with internet addresses. sockaddr_in compitable only with IPV4 addresses */
    struct sockaddr_in server_addr, client_addr;

    socklen_t from_length = 0;
    int ret_num_bytes = 0;

/* 1. */
    /* Creating a socket file descriptor. param:    domain, type UDP, protocol 
    Use PF_INET in the call to socket(). and AD_INET in the struct */
    if ((sock_fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        exit(EXIT_FAILURE);
    }

    /* Reset the structs */
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    /* Filling server Information */
    server_addr.sin_family = AF_INET;   /* IPV4 */
    /* Convert the port number to newtwork byte order */
    server_addr.sin_port = htons(PORT);
    /* INADDR_ANY IP is automatically being selected */
    server_addr.sin_addr.s_addr = INADDR_ANY;

/* 2. */
    /* Bind the socket with the server address */
    if (bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        exit(EXIT_FAILURE);
    }

/* 3. */
    /* from_length will contain the length of the address actually stored in 
    client_addr */
    from_length = sizeof(client_addr);

    /* Receive msg from the Client */
    ret_num_bytes = recvfrom(sock_fd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&client_addr, &from_length);

    buffer[ret_num_bytes] = '\0';
    printf("Client Sent: %s\n", buffer);

    sendto(sock_fd, msg, strlen(msg), MSG_CONFIRM, (struct sockaddr*)&client_addr, from_length);

    return 0;
}