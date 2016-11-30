
#ifndef _SERVER_H_ 
#define _SERVER_H_ 

/*
    Simple udp server
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFLEN 8  //Max length of buffer
#define Endfile 26  // EOF

/* XON/XOFF protocol */ 
#define XON (0x11) 
#define XOFF (0x13) 

/* Const */ 
#define BYTESIZE 256 // The maximum value of a byte
#define MAXLEN 1024 // Maximum messages length

#define MIN_UPPERLIMIT 5
#define MAX_LOWERLIMIT 2

/* Delay to adjust speed of consuming buffer, in milliseconds */
#define DELAY 500

typedef unsigned char Byte;
struct bufer {
 	Byte data[BUFLEN];
 	int dummy;
} bufer[BUFLEN];



void die(char *s);
void *childRProcess(void * threadid);


#endif