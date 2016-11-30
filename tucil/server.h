
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

void die(char *s);


#endif