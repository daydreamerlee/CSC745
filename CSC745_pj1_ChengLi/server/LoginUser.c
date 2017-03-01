//  LoginUser.c
//  server
//
//  Created by Xinlu Chen on 10/17/16.
//  Copyright © 2016 CSC745. All rights reserved.
//

#include <stdio.h> /* for printf() and fprintf() */
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#include "ClientInfo.h"

#define SENDBUFSIZE 30 /* Size of send buffer */
#define MAXSIZE 20 /* Max size of username and password */
#define USERNUM 2 /* Number of users */


struct clientinfo *LoginUser(int clntSock, struct clientinfo *users)
{
    int recvMsgSize; /* Size of received message */
    
    char username[MAXSIZE];
    if ((recvMsgSize = recv(clntSock, username, MAXSIZE - 1, 0)) < 0)
        DieWithError("recv() failed");
    username[recvMsgSize] = '\0';
    
    char password[MAXSIZE];
    if ((recvMsgSize = recv(clntSock, password, MAXSIZE - 1, 0)) < 0)
        DieWithError("recv() failed");
    password[recvMsgSize] = '\0';
    
    /* Print user input information */
    printf("user <%s>", username);
    printf(" is trying to login with password <%s>.\n", password);
    
    /* Find user by login infomation */
    struct clientinfo defaultUser = { "", "", -1, false, "" };
    struct clientinfo *currentUser = &defaultUser;
    char notice[SENDBUFSIZE] = "Login success!";
    int i;
    for (i = 0; i < USERNUM; i++) {
        /* Compare the user input with the data stored on server */
        if (strcmp(username, users[i].username) == 0) { /* Username matched */
            if (strcmp(password, users[i].password) == 0) { /* Password matched */
                currentUser = &users[i];
                currentUser->clntSocket = clntSock;
                currentUser->loggedin = true;
                break;
            } else { /* Password doesn't match */
                strcpy(notice, "Wrong password");
                //printf("Wrong password <%s> with username <%s>.\n", password, username);
            }
        }
    }
    /* Print out login result */
    if (currentUser->loggedin == false) { /* No matched-username client found */
        strcpy(notice, "Login failed!");
        printf("%s\n", notice);
        //printf("Falied to find user with username <%s> on server.\n", username);
    } else {
        printf("Login success!\n");
        printf("Login username is <%s>.\n", username);
        printf("Login password is <%s>.\n", password);
    }
    
    /* Send login notice to client */
    if (send(clntSock, &notice, sizeof(notice), 0) < 0)
        DieWithError("send() failed");
    
    return currentUser;
}

