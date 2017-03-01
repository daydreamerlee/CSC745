#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#include <string.h>
#include "ClientInfo.h"

#define BUFSIZE 200 /* Size of receive buffer */
#define USERNUM 2 /* Number of users */
#define USERBUF 100 /* Size of userlist buffer */


void DieWithError(char *errorMessage); /* Error handling function */

void HandleTCPClient(struct clientinfo *users, struct clientinfo *currentUser, int clntSocket) {
    char recvBuffer[BUFSIZE]; /* Buffer for received message */
    char sendBuffer[BUFSIZE]; /* Buffer for sending message */
    int recvMsgSize; /* Size of received message */
    
    while (1) {
        /* Receive message from client */
        if ((recvMsgSize = recv(clntSocket, recvBuffer, BUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
        recvBuffer[recvMsgSize] = '\0';
        
        /* Get the user list by command 1 */
        if (strcmp(recvBuffer, "Option 1") == 0) {
            printf("Return user list!\n");
            int i;
            char userList[USERBUF] = "There are 2 users: \n";
            for (i = 0; i < USERNUM; i++) {
                strcat(userList, users[i].username);
                strcat(userList, "\n");
            }
            /* Send the user list to the client */
            if (send(clntSocket, userList, strlen(userList), 0) != strlen(userList))
                DieWithError("send() failed");
        }
        
        /* Send a message by command 2 */
        else if (strcmp(recvBuffer, "Option 2") == 0) {
            /* Receive the target username from client */
            if ((recvMsgSize = recv(clntSocket, recvBuffer, BUFSIZE, 0)) < 0)
                DieWithError("recv() failed");
            recvBuffer[recvMsgSize] = '\0';
            
            int i;
            struct clientinfo *targetUser;
            /* Find the target user */
            for (i = 0; i < USERNUM; i++) {
                if (strcmp(users[i].username, recvBuffer) == 0) {
                    targetUser = &users[i];
                    printf("Got a message to <%s>\n", recvBuffer);
                    break;
                }
            }
            /* Receive the target message from client */
            if ((recvMsgSize = recv(clntSocket, recvBuffer, BUFSIZE, 0)) < 0)
                DieWithError("recv() failed");
            recvBuffer[recvMsgSize] = '\0';
            printf("The message is <%s>\n", recvBuffer);
            strcat(targetUser->msgBuffer, recvBuffer); /* Store the message under the target username */
            
            strcpy(sendBuffer, "Message sent successfully!");
            printf("%s\n", sendBuffer);
            /* Send the successful notice to the client */
            if (send(clntSocket, sendBuffer, strlen(sendBuffer), 0) != strlen(sendBuffer))
                DieWithError("send() failed");
        }
        
        /* Get user's messages by command 3 */
        else if (strcmp(recvBuffer, "Option 3") == 0) {
            printf("Send back <%s> message!\n", currentUser->username);
            /* Check if user has messages */
            if (strcmp(currentUser->msgBuffer, "") == 0) {
                printf("There is no message for <%s>.\n", currentUser->username);
            } else {
                printf("Sending out messages: <%s>\n", currentUser->msgBuffer);
            }
            /* Send user's message to the client */
            if (send(clntSocket, currentUser->msgBuffer, strlen(currentUser->msgBuffer), 0)
                != strlen(currentUser->msgBuffer))
                DieWithError("send() failed");
        }
        
        /* Initiate a chat with friend by command 4 */
        else if (strcmp(recvBuffer, "Option 4") == 0) {
            printf("Client disconnected!\n\n");
            close(clntSocket); /* Close the socket that handles client */
            break;
        }
        
        /* Chat with friend by command 5 */
        else if (strcmp(recvBuffer, "Option 5") == 0) {
            printf("Client disconnected!\n\n");
            close(clntSocket); /* Close the socket that handles client */
            break;
        }
        strcpy(recvBuffer, ""); /* Set recvBuffer back to an empty string */
        strcpy(sendBuffer, ""); /* Set sendBuffer back to an empty string */
    }
}



