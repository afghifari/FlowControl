/* 
 *	File : receiver.h
 */ 

#ifndef _RECEIVER_H_ 
#define _RECEIVER_H_ 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define CR 13 // return
#define LF 10 // line feed
#define Endfile 26 // EOF character

/* XON/XOFF protocol */ 
#define XON (0x11) 
#define XOFF (0x13) 

/* Const */ 
#define BYTESIZE 256 // The maximum value of a byte
#define MAXLEN 1024 // Maximum messages length
#define BUFMAX 8	

#define MIN_UPPERLIMIT 5
#define MAX_LOWERLIMIT 2

#define bzero(p, size) (void)memset((p), 0 , (size))
/* Delay to adjust speed of consuming buffer, in milliseconds */
#define DELAY 500
/* Define receive buffer size */
#define RXQSIZE 8

typedef char Byte;
typedef struct QTYPE {
 	unsigned int count;
 	unsigned int front;
 	unsigned int rear;
 	unsigned int maxsize;
 	Byte *data;
} QTYPE;

// typedef struct MESGB {
//  	unsigned int soh;
//  	unsigned int stx;
//  	unsigned int etx;
//  	Byte checksum;
//  	Byte msgno;
//  	Byte *data;
// } MESGB;

/* FUNCTIONS AND PROCEDURES */
static Byte *rcvchar(int sockfd, QTYPE *queue);

static Byte *q_get(QTYPE *, Byte *);

void *childRProcess(void * threadid);

void error(const char* message);

#endif