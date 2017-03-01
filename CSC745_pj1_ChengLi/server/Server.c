#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "ClientInfo.h"

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define SERVPORT 8000
#define USERNUM 2 /* Number of users */

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(struct clientinfo *users, struct clientinfo *currentUser,
                     int newSocket); /* TCP client handling function */
struct clientinfo *LoginUser(int newSock, struct clientinfo *users);

int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int newSock; /* Socket descriptor for client */
    struct sockaddr_in chatServAddr; /* Local address */
    struct sockaddr_in chatClntAddr; /* Client address */
    unsigned short chatServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */

    /* Hardcode userlist, skip register for now */
    struct clientinfo users[USERNUM] = { { "Alice", "12345", -1, false, "" },
                                        { "Bob", "56789", -1, false, "" } };
    chatServPort = SERVPORT;
    
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;

    /* Construct local address structure */
    memset(&chatServAddr, 0, sizeof(chatServAddr)); /* Zero out structure */
    chatServAddr.sin_family = AF_INET; /* Internet address family */
    chatServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    chatServAddr.sin_port = htons(chatServPort); /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&chatServAddr, sizeof(chatServAddr)) < 0)
        DieWithError ( "bind () failed");
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0) {
        DieWithError("listen() failed") ;
    } else {
        printf("Server started!\n");
        printf("Listen on 127.0.0.1: %d\n", chatServPort);
    }
    
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(chatClntAddr);
        /* Wait for a client to connect */
        if ((newSock = accept(servSock, (struct sockaddr *) &chatClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        /* newSock is connected to a client! */
        printf("Client Connected!\n");
        
        /* Get the current login client */
        struct clientinfo *currentUser = LoginUser(newSock, users);
        if(currentUser->loggedin == false){
            close(newSock);
        } else {
            HandleTCPClient (users, currentUser, newSock);
        } 
    }
/* NOT REACHED */
}
