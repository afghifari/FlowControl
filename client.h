#ifndef CLIENT_H
#define CLIENT_H

/*
    Simple udp client
*/
#include <string.h> //memset
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h> 

#define XON (0x11) 
#define XOFF (0x13) 

// konstanta 
#define BYTESIZE 256
#define MAXLEN 1024 
#define BUFLEN 8  //Max length of buffer
#define Endfile 26  // EOF
#define MAXFRAME 5s

struct frame {
	int seqNum;
	int ackNum;
	int flag;
	char data[BUFLEN+1];
} frame[MAXFRAME];

void error(const char *message);

void *childProcess(void *threadid);









#endif