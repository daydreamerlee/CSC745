#ifndef ClientInfo_h
#define ClientInfo_h

#define RCVMSGBUFSIZE 2500 /* Size of received message buffer */
#define MAXSIZE 20 /* Max size of username and password */

typedef int bool;
enum { false, true };

struct clientinfo
{
    char username[MAXSIZE];
    char password[MAXSIZE];
    int clntSocket;
    bool loggedin;
    char msgBuffer[RCVMSGBUFSIZE];
};

#endif /* ClientInfo_h */
