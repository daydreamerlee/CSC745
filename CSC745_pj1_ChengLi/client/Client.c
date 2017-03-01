#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define BUFSIZE 200 /* Size of both receive and send buffer */
#define MAXIP 16 /* Max size of IP address */
#define MAXSIZE 20 /* Max size of username and password */
#define MAXPENDING 5 /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage); /* Error handling function */
int GetClientOptionFromMenu(); /* Show the chatting menu and return client option */

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    int servSock; /* Socket descriptor as a server */
    struct sockaddr_in chatServAddr; /* Chat server address */
    unsigned short chatServPort; /* Chat server port */
    char servIP[MAXIP]; /* Server IP address (dotted quad) */
    char username[MAXSIZE]; /* Client login username */
    char password[MAXSIZE]; /* Client login password*/
    char recvBuffer[BUFSIZE]; /* Buffer for received string */
    char sendBuffer[BUFSIZE]; /* Buffer string send to chat server */
    int bytesRcvd; /* Bytes read in single recv() */

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket () failed") ;
    
    while(1) {
        /* Get user's option from menu */
        int userOption = GetClientOptionFromMenu();
        printf("You chose option %d\n\n", userOption);

        switch (userOption) {
        case 0:
            printf("Please enter the IP address: ");
            scanf("%s", servIP);
            printf("Please enter the port number: ");
            scanf("%hu", &chatServPort);
                
            /* Construct the server address structure */
            memset(&chatServAddr, 0, sizeof(chatServAddr)); /* Zero out structure */
            chatServAddr.sin_family = AF_INET; /* Internet address family */
            chatServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
            chatServAddr.sin_port = htons(chatServPort); /* Server port */
                
            /* Establish the connection to the chat server */
            if (connect(sock, (struct sockaddr *) &chatServAddr, sizeof(chatServAddr)) < 0)
                DieWithError("connect () failed");
            printf("Connecting...\nConnected!\n\n");
            printf("Welcome! Please log in.\n");
            printf("Username: ");
            scanf("%s", username);
            /* Send username to the server */
            if (send(sock, &username, strlen(username),0) != strlen(username))
                DieWithError("send() sent a different number of bytes than expected\n");
    
            printf("Password: ");
            scanf("%s", password);
            /* Send password to the server */
            if (send(sock, &password, strlen(password),0) != strlen(password))
                DieWithError("send() sent a different number of bytes than expected\n");
                
            /* Receive login notice from the server */
            if ((bytesRcvd = recv(sock, &recvBuffer, BUFSIZE - 1, 0)) < 0)
                DieWithError("recv() failed");
            recvBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
            printf("%s\n", recvBuffer);
            break;
            
        case 1:
            strcpy(sendBuffer, "Command 1"); /* Copy address of "Command 1" to sendBuffer */
            /* Send the command 1 to the server */
            if (send(sock, &sendBuffer, strlen(sendBuffer),0) != strlen(sendBuffer))
                DieWithError("send() sent a different number of bytes than expected\n");
            /* Receive the user list from the server */
            if ((bytesRcvd = recv(sock, &recvBuffer, BUFSIZE - 1, 0)) < 0)
                DieWithError("recv() failed");
            recvBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
            printf("%s\n", recvBuffer);
            break;
        
        case 2:
            strcpy(sendBuffer, "Command 2"); /* Copy address of "Command 2" to sendBuffer */
            /* Send the command 2 to the server */
            if (send(sock, &sendBuffer, strlen(sendBuffer),0) != strlen(sendBuffer))
                DieWithError("send() sent a different number of bytes than expected\n");
                
            char targetUsername[MAXSIZE];
            printf("Please enter the target username: ");
            scanf("%s", targetUsername);
            /* Send the target username to the server */
            if (send(sock, &targetUsername, strlen(targetUsername),0) != strlen(targetUsername))
                DieWithError("send() sent a different number of bytes than expected\n");
            
            /* Get rid of a new line charactor */
            int c;
            while ((c = getchar()) != EOF && c != '\n');
                
            printf("Please enter the target message: ");
            fgets(sendBuffer, sizeof(sendBuffer), stdin); /* Read the whole line including empty space */
            sendBuffer[strcspn(sendBuffer, "\n")] = '\0';
            /* Send the target message to the server */
            if (send(sock, &sendBuffer, strlen(sendBuffer),0) != strlen(sendBuffer))
                DieWithError("send() sent a different number of bytes than expected\n");
            
            /* Receive the notice from the server */
            if ((bytesRcvd = recv(sock, &recvBuffer, BUFSIZE - 1, 0)) < 0)
                DieWithError("recv() failed");
            recvBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
            printf("%s\n", recvBuffer);
            break;
        
        case 3:
            strcpy(sendBuffer, "Command 3"); /* Copy address of "Command 3" to sendBuffer */
            /* Send the command 3 to the server */
            if (send(sock, &sendBuffer, strlen(sendBuffer),0) != strlen(sendBuffer))
                DieWithError("send() sent a different number of bytes than expected\n");
            
            /* Receive user's message from the server */
            if ((bytesRcvd = recv(sock, &recvBuffer, BUFSIZE - 1, 0)) < 0) {
                DieWithError("recv() failed");
            } else if (bytesRcvd == 0) {
                printf("There is no message for you.\n");
            } else {
                recvBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
                printf("The message is <%s>\n", recvBuffer);
            }
            break;
        
        case 4:
            strcpy(sendBuffer, "Command 4"); /* Copy address of "Command 4" to sendBuffer */
            /* Send the command 4 to the server */
            if (send(sock, &sendBuffer, strlen(sendBuffer),0) != strlen(sendBuffer))
                DieWithError("send() sent a different number of bytes than expected\n");
            close(sock); /* Close client socket */
            printf("----- Disconnect with the server -----\n");
            
            /* This user becomes a server and waits another user to connect */
            struct sockaddr_in addr; /* Address as a server */
            printf("Please enter the port number you want to listen on: ");
            int listenPort;
            scanf("%d", &listenPort);
                
            /* Create socket for incoming connections */
            if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                DieWithError( "socket () failed") ;
                
            /* Construct local address structure */
            memset(&addr, 0, sizeof(addr)); /* Zero out structure */
            addr.sin_family = AF_INET; /* Internet address family */
            addr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
            addr.sin_port = htons(listenPort); /* Local port */
                
            /* Bind to the local address */
            if (bind(servSock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                DieWithError ( "bind () failed");
            /* Mark the socket so it will listen for incoming connections */
            if (listen(servSock, MAXPENDING) < 0) {
                DieWithError("listen() failed") ;
            } else {
                printf("I am listening on 127.0.0.1: %d\n", listenPort);
            }
            
            /* Get rid of a new line charactor */
            while ((c = getchar()) != EOF && c != '\n');
                
            int newSock, clntLen;
            struct sockaddr_in clntAddr;
            for (;;) /* Run forever */
            {
                /* Set the size of the in-out parameter */
                clntLen = sizeof(clntAddr);
                /* Wait for a client to connect */
                if ((newSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0)
                    DieWithError("accept() failed");
                /* newSock is connected to a client! */
                printf("Client Connected!\n");
                
                char recvMessage[BUFSIZE];
                char sendMessage[BUFSIZE];
                char line[BUFSIZE];
                while (1) {
                    printf("< Type 'bye' to stop the converstation >\n");
                    /* Receive the message from the client (friend) */
                    if ((bytesRcvd = recv(newSock, &recvMessage, BUFSIZE - 1, 0)) < 0)
                        DieWithError("recv() failed");
                    recvMessage[bytesRcvd] = '\0'; /* Terminate the string! */
                    printf("%s\n", recvMessage);
                    
                    if (strcmp(recvMessage, "bye") == 0) {
                        close(newSock); /* Close the socket that handles client */
                        printf("----- Disconnect from my friend -----\n");
                        break;
                    }
                    
                    printf("%s: ", username);
                    fgets(sendMessage, sizeof(sendMessage), stdin);
                    sendMessage[strcspn(sendMessage, "\n")] = '\0';
                    
                    if (strcmp(sendMessage, "bye") == 0) {
                        if (send(newSock, &sendMessage, strlen(sendMessage),0) != strlen(sendMessage))
                            DieWithError("send() sent a different number of bytes than expected\n");
                        close(newSock); /* Close the socket that handles client */
                        printf("----- Disconnect from my friend -----\n");
                        break;
                    } else {
                        /* Store the username with the message together to sendBuffer */
                        strcpy(line, username);
                        strcat(line, ": ");
                        strcat(line, sendMessage);
                        /* Send the message to the client (friend) */
                        if (send(newSock, &line, strlen(line),0) != strlen(line))
                            DieWithError("send() sent a different number of bytes than expected\n");
                    }
                    strcpy(recvMessage, "");
                    strcpy(sendMessage, "");
                    strcpy(line, "");
                }
            }
            break;
                
        case 5:
            strcpy(sendBuffer, "Command 5"); /* Copy address of "Command 5" to sendBuffer */
            /* Send the command 4 to the server */
            if (send(sock, &sendBuffer, strlen(sendBuffer),0) != strlen(sendBuffer))
                DieWithError("send() sent a different number of bytes than expected\n");
            close(sock); /* Close client socket */
            printf("----- Disconnect with the server -----\n");
                
            /* The user tries to connect to another user */
            printf("Please enter your friend's IP address: ");
            scanf("%s", servIP);
            printf("Please enter your friend's port number: ");
            scanf("%hu", &chatServPort);
                
            /* Create a reliable, stream socket using TCP */
            if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                DieWithError("socket () failed") ;
                
            /* Construct the server address structure */
            memset(&chatServAddr, 0, sizeof(chatServAddr)); /* Zero out structure */
            chatServAddr.sin_family = AF_INET; /* Internet address family */
            chatServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
            chatServAddr.sin_port = htons(chatServPort); /* Server port */
                
            /* Establish the connection to the chat server */
            if (connect(sock, (struct sockaddr *) &chatServAddr, sizeof(chatServAddr)) < 0)
                DieWithError("connect () failed");
            printf("Connecting your friend...\nConnected!\n\n");
            
            /* Get rid of a new line charactor */
            while ((c = getchar()) != EOF && c != '\n');
                
            char recvMessage[BUFSIZE];
            char sendMessage[BUFSIZE];
            char line[BUFSIZE];
            while (1) {
                printf("< Type 'bye' to stop the converstation >\n");
                
                printf("%s: ", username);
                fgets(sendMessage, sizeof(sendMessage), stdin);
                sendMessage[strcspn(sendMessage, "\n")] = '\0';
                
                if (strcmp(sendMessage, "bye") == 0) {
                    if (send(sock, &sendMessage, strlen(sendMessage),0) != strlen(sendMessage))
                        DieWithError("send() sent a different number of bytes than expected\n");
                    close(sock); /* Close client socket */
                    printf("----- Disconnect from my friend -----\n");
                    break;
                } else {
                    /* Store the username with the message together to sendBuffer */
                    strcpy(line, username);
                    strcat(line, ": ");
                    strcat(line, sendMessage);
                    /* Send the message to the client (friend) */
                    if (send(sock, &line, strlen(line),0) != strlen(line))
                        DieWithError("send() sent a different number of bytes than expected\n");
                }
                
                /* Receive the message from the server (friend) */
                if ((bytesRcvd =recv(sock, &recvMessage, BUFSIZE - 1, 0)) < 0)
                    DieWithError("recv() failed");
                recvMessage[bytesRcvd] = '\0'; /* Terminate the string! */
                printf("%s\n", recvMessage);
                
                if (strcmp(recvMessage, "bye") == 0) {
                    close(sock); /* Close client socket */
                    printf("----- Disconnect from my friend -----\n");
                    break;
                }
                strcpy(recvMessage, ""); /* Set recvMessage back to an empty string */
                strcpy(sendMessage, ""); /* Set sendMessage back to an empty string */
                strcpy(line, "");
            }
            break; /* Break switch() */
        }
        
        strcpy(recvBuffer, ""); /* Set recvBuffer back to an empty string */
        strcpy(sendBuffer, ""); /* Set sendBuffer back to an empty string */
    }
}

int GetClientOptionFromMenu() {
    int option;
    printf("----------------------------------------\n");
    printf("Menu\n");
    printf("0. Connect to the server\n");
    printf("1. Get the user list\n");
    printf("2. Send a message\n");
    printf("3. Get my messages\n");
    printf("4. Initiate a chat with my friend\n");
    printf("5. Chat with my friend\n");
    
    printf("Your option < enter a number>: ");
    scanf("%d", &option);
    return option;
}
