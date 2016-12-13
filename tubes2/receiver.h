
#ifndef _RECEIVER_H_ 
#define _RECEIVER_H_ 

/*
    Simple udp RECEIVER
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN 8  //Max length of buffer
#define Endfile 26  // EOF
#define SOH 1
#define STX 2
#define ETX 3
#define ENQ 5
#define BEL 7
#define CR 13 // return
#define LF 10 // line feed
#define NAK 21
#define Endfile 26 // EOF character
#define ESC



/* XON/XOFF protocol */ 
#define XON (0x11) 
#define XOFF (0x13) 
#define ACK 6

/* Const */ 
#define BYTESIZE 256 // The maximum value of a byte
#define MAXLEN 1024 // Maximum messages length

#define MIN_UPPERLIMIT 5
#define MAX_LOWERLIMIT 2

/* Delay to adjust speed of consuming buffer, in milliseconds */
#define DELAY 500

typedef unsigned char Byte;
struct bufer {
	int ackFlag;
 	Byte data[BUFLEN+1+1+4];
} bufer[BUFLEN];



void die(char *s);
void *childRProcess(void * threadid);


#endif