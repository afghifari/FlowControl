/*
    Simple udp client
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define SERVER "10.5.13.186"
#define BUFLEN 8  //Max length of buffer
#define PORT 20000   //The port on which to send data
#define MYFILE "file.txt"

void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    int fs_block_sz;
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    //while(1)
    //{
        /*==================================================*/

        FILE *fs = fopen(MYFILE, "r");
        if(fs == NULL)
        {
            printf("ERROR: File %s not found.\n", MYFILE);
            exit(1);
        }


        bzero(buf, BUFLEN); 
         
       
        /*==================================================*/



        //gets(message);
        
        while((fs_block_sz = fread(buf, sizeof(char), BUFLEN, fs)) > 0)
        {
            printf("isi = %s\n", buf);
            //send the message
            if (sendto(s, buf, fs_block_sz , 0 , (struct sockaddr *) &si_other, slen) == -1)
            {
                fprintf(stderr, "ERROR: Failed to send file %s. \n", MYFILE);
                die("sendto()");
            }

            bzero(buf, BUFLEN);
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        // memset(buf,'\0', BUFLEN);
        // //try to receive some data, this is a blocking call
        // if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        // {
        //     die("recvfrom()");
        // }
         
        // puts(buf);
    //}
 
    close(s);
    return 0;
}