 /* 
  * File 	: transmitter.h
  */

#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h> 

#define CR 13 		// Return
#define LF 10 		// NewLine 
#define Endfile 26 	// EOF


#define XON (0x11) 
#define XOFF (0x13) 

// konstanta 
#define BYTESIZE 256
#define MAXLEN 1024 
#define BUFMAX 1	

void error(const char *message);

void *childProcess(void *threadid);

#endif