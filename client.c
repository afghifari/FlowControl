#include "client.h"


struct hostent *SERVER;
struct sockaddr_in si_other;
int slen=sizeof(si_other);
int s, i;
char buf[BUFLEN];
char message[BUFLEN];
int fs_block_sz;
char xbuf[BUFLEN+1];

/* FLAGS */
int isXON = 1;          // flag for XON/XOFF sent
int isSocketOpen;       // flag to indicate if connection from socket is done

void die(char *s)
{
    perror(s);
    exit(1);
}


void *childProcess(void *threadid) {
    // Mengecek XON/XOFF
    struct sockaddr_in srcAddr;
    int srcLen = sizeof(srcAddr);
    while (isSocketOpen) {
        if (recvfrom(s, xbuf, 1, 0, (struct sockaddr *) &srcAddr, &srcLen) != 1)
            error("ERROR: ukuran buffer besar\n");

        if (xbuf[0] == XOFF) {
            isXON = 0;
            printf("XOFF diterima.\n");
        } else {                // xbuf[0] == XON
            isXON = 1;
            printf("XON diterima.\n");
        }
    }
    pthread_exit(NULL);
}


 
int main(int argc, char *argv[]) {
    pthread_t thread[1];
    int counter = 1;

    // cek argumen
    if (argc < 4) {
        printf("Usage: %s <target-ip> <port> <filename>\n", argv[0]);
        return 0;
    }

    // get server
    if ((SERVER = gethostbyname(argv[1])) == NULL)
        error("ERROR: Alamat receiver salah.\n");

 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    // inisialisasi socket host
    memset(&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    bcopy((char *)SERVER->h_addr, (char *)&si_other.sin_addr.s_addr, SERVER->h_length);
    si_other.sin_port = htons(atoi(argv[2]));


    FILE *fs = fopen(argv[argc-1], "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", argv[argc-1]);
        exit(1);
    }
    memset(buf,'\0', BUFLEN);

    // flag=1 -> connection is established
    isSocketOpen = 1;

   
    if (pthread_create(&thread[0], NULL, childProcess, 0) != 0) 
        error("ERROR: Gagal membuat thread.\n");
    char aa[5]="aku";
    char bb[6]="kamuX";
    //strcat(aa,bb);
    aa[3]=4+'0';
    aa[4]=5+'0';
    aa[5]='\0';
    printf("aa = %s\n",aa);
    int frameNum = 0;

    while((fs_block_sz = fread(buf, sizeof(char), BUFLEN, fs)) > 0)
    {
        buf[BUFLEN]='\0';

        if (isXON) {
            //send the message
            if (sendto(s, buf, fs_block_sz , 0 , (struct sockaddr *) &si_other, slen) == -1)
            {
                fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
                die("sendto()");
            }
            printf("Mengirim frame ke-%d : %s\n", counter++, buf);

        } else {
            while (!isXON) {
                printf("Menunggu XON...\n");
                sleep(1);
            }

            if (isXON) {
                //send the message
                if (sendto(s, buf, fs_block_sz , 0 , (struct sockaddr *) &si_other, slen) == -1)
                {
                    fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
                    die("sendto()");
                }
                printf("Mengirim frame ke-%d : %s\n", counter++, buf);
            }

        }

        memset(buf,'\0', BUFLEN);
        sleep(1);
    }

    memset(buf,'\0', BUFLEN);
    printf("Send EOF\n");
    buf[0]=Endfile;
    buf[1]='\0';
    if (sendto(s, buf, 2 , 0 , (struct sockaddr *) &si_other, slen) == -1)
    {
        fprintf(stderr, "ERROR: Failed to send EOF character\n");
        die("sendto()");
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

    fclose(fs);
    
    printf("Karakter selesai dikirim, menutup socket\n");
    
    isSocketOpen = 0;
    close(s);
    printf("Socket ditutup!\n");

    printf("Finish!\n");
    return 0;
}
