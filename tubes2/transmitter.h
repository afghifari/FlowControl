#ifndef TRANSMITTER_H
#define TRANSMITTER_H

/*
    Simple udp TRANSMITTER
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


/*ASCII Const*/
#define SOH 1
#define STX 2
#define ETX 3
#define ENQ 5
#define ACK '6'
#define BEL 7
#define CR 13 // return
#define LF 10 // line feed
#define NAK 21
#define Endfile 26 // EOF character
#define ESC

/* XON/XOFF protocol */ 
#define XON (0x11) 
#define XOFF (0x13) 

// konstanta 
#define BYTESIZE 256
#define MAXLEN 1024 
#define BUFLEN 8  //Max length of buffer
#define Endfile 26  // EOF
#define MAXFRAME 4

struct window {
	int ackFlag;
	char data[BUFLEN+1];
	int dummy;
} window[MAXFRAME];

void error(const char *message);

void *childProcess(void *threadid);









#endif