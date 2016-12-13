#include "transmitter.h"


struct hostent *SERVER;
struct sockaddr_in si_other;
int slen=sizeof(si_other);
int s, i;
char buf[BUFLEN];
char message[BUFLEN];
int fs_block_sz;
char xbuf[BUFLEN];
char aceka[2];
int counterACK;

/* FLAGS */
int isXON = 1;          // flag for XON/XOFF sent
int isSocketOpen;       // flag to indicate if connection from socket is done
int isAckReceived;

void die(char *s)
{
    perror(s);
    exit(1);
}

void *childProcess(void *threadid) {
    // Mengecek XON/XOFF dan ACK
    while (isSocketOpen) {
        memset(xbuf,'\0', BUFLEN);
        if (recvfrom(s, xbuf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
            error("ERROR: ukuran buffer besar\n");  

        if (xbuf[0] == ACK){
            int num = xbuf[1] - '0';
            window[num].ackFlag=1;
        } else if (xbuf[0] == NAK){
            printf("NAK diterima\n");
        } else if (xbuf[0] == XOFF) {
            isXON = 0;
            printf("XOFF diterima.\n");
        } else if (xbuf[0] == XON){                // xbuf[0] == XON
            isXON = 1;
            printf("XON diterima.\n");
        }
    }
    pthread_exit(NULL);
}

 
int main(int argc, char *argv[]) {
    pthread_t thread[2];
    int counter = 0;
    int frameNum = 0;
    int satuan;
    int puluhan;
    int ratusan;

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
    isAckReceived = 1;

    if (pthread_create(&thread[0], NULL, childProcess, 0) != 0) 
        error("ERROR: Gagal membuat thread 1.\n");

    for (i=0;i<4;i++) {
        window[i].ackFlag=0;
    }
    int ii;
    int jumbit=0;
    while((fs_block_sz = fread(buf, sizeof(char), BUFLEN, fs)) > 0)
    {
        buf[BUFLEN]='\0';   
        jumbit = 0;
        if (isXON) {
            //send the message
            memset(window[counter].data,'\0', BUFLEN+1+1+4);
            
            for(ii=0;ii<BUFLEN;ii++) {
                window[counter].data[ii+1] = buf[ii];
                jumbit+=buf[ii];
            }
            //printf("jumbit = %d \n", jumbit);




            //strncpy(window[counter].data, buf, BUFLEN);
            printf("Mengirim frame ke-%d : %s\n", counter, buf);
            window[counter].data[9] = counter + '0';

            int ribuan = jumbit / 1000;
            int ratusan = (jumbit / 100) % 10;
            int puluhan = (jumbit / 10 ) % 10;
            int satuan = jumbit % 10;
            window[counter].data[10] = ribuan + '0';
            window[counter].data[11] = ratusan + '0';
            window[counter].data[12] = puluhan + '0';
            window[counter].data[13] = satuan + '0';




            window[counter].data[0] = fs_block_sz+1 + '0';
            window[counter].data[BUFLEN+1+1+4] = '\0';

            //printf("datalength = %c\n", window[counter].data[0]);


            if (sendto(s, window[counter].data, fs_block_sz+1+1+4 , 0 , (struct sockaddr *) &si_other, slen) == -1)
            {
                fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
                die("sendto()");
            }
            

            counter++;
        } else {
            while (!isXON) {
                printf("Menunggu XON...\n");
                sleep(1);
            }

            if (isXON) {
                //send the message
                memset(window[counter].data,'\0', BUFLEN+1+1+4);
            

                for(ii=0;ii<BUFLEN;ii++) {
                    window[counter].data[ii+1] = buf[ii];
                    jumbit+=buf[ii];
                }
                //strncpy(window[counter].data,buf,BUFLEN);
                printf("Mengirim frame ke-%d : %s\n", counter, buf);
                window[counter].data[9] = counter + '0';

                int ribuan = jumbit / 1000;
                int ratusan = (jumbit / 100) % 10;
                int puluhan = (jumbit / 10 ) % 10;
                int satuan = jumbit % 10;
                window[counter].data[10] = ribuan + '0';
                window[counter].data[11] = ratusan + '0';
                window[counter].data[12] = puluhan + '0';
                window[counter].data[13] = satuan + '0';


                window[counter].data[0] = fs_block_sz+1 + '0';
                window[counter].data[BUFLEN+1+1+4] = '\0';


                if (sendto(s, window[counter].data, fs_block_sz+1+1+4 , 0 , (struct sockaddr *) &si_other, slen) == -1)
                {
                    fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
                    die("sendto()");
                }
                
                counter++;
            }

        }

        if (counter==MAXFRAME) {
            printf("Menunggu Acknowledgement\n");
            sleep(10);
            int hit=0;
            int cek=1;
            while(hit<MAXFRAME){
                if (window[hit].ackFlag==0)
                    cek=0;
                hit++;
            }
            if (cek==0) {

                char messages[BUFLEN];
                memset(messages,'\0', BUFLEN);

                while(cek==0){
                    printf("ACK tidak diterima dengan baik\n");
                    printf("Mengirim pesan ulang..\n");
                    hit=0;
                    while(hit<MAXFRAME) {
                        for (i=1;i<=BUFLEN;i++) {
                            messages[i-1] = window[hit].data[i];
                        }
                        if (isXON) {
                            if (sendto(s, window[hit].data, fs_block_sz+1+1+4 , 0 , (struct sockaddr *) &si_other, slen) == -1)
                            {
                                fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
                                die("sendto()");
                            }
                            printf("Mengirim frame ke-%d : %s\n", hit, messages);
                        } else {
                            while (!isXON) {
                                printf("Menunggu XON...\n");
                                sleep(1);
                            }
                            if (sendto(s, window[hit].data, fs_block_sz+1+1+4 , 0 , (struct sockaddr *) &si_other, slen) == -1)
                            {
                                fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
                                die("sendto()");
                            }
                            printf("Mengirim frame ke-%d : %s\n", hit, messages);
                        }
                        hit++;
                        sleep(1);
                    }
                    
                    printf("Menunggu Acknowledgement\n");
                    sleep(10);
                    
                    cek=1;
                    hit=0;
                    while(hit<MAXFRAME){
                        if (window[hit].ackFlag==0)
                            cek=0;
                        hit++;
                    }
                }

            }

            for (i=0;i<MAXFRAME;i++) {
                window[i].ackFlag=0;
            }

            counter=0;
        }
        
        sleep(1);
    }
    //sleep(10);
    // printf("cek awal\n");
    // // kasus khusus
    // if (counter<MAXFRAME) {
    //     printf("Menunggu Acknowledgement -\n");

    //     sleep(10);
    //     printf("oke\n");
    //     int hit=0;
    //     int cek=1;
    //     printf("cek0\n");
    //     while(hit<counter){
    //         if (window[hit].ackFlag==0)
    //             cek=0;
    //         hit++;
    //     }
    //     printf("cek1\n");
    //     if(cek==0) {
    //         while(cek==0) {
    //             printf("\nACK tidak diterima dengan baik\n");
    //             printf("Mengirim pesan ulang..\n");
    //             hit=0;
    //             while(hit<counter) {
    //                 if (isXON) {
    //                     if (sendto(s, window[hit].data, fs_block_sz+1 , 0 , (struct sockaddr *) &si_other, slen) == -1)
    //                     {
    //                         fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
    //                         die("sendto()");
    //                     }
    //                     printf("Mengirim frame ke-%d : %s\n", hit, window[hit].data);
    //                 } else {
    //                     while (!isXON) {
    //                         printf("Menunggu XON...\n");
    //                         sleep(1);
    //                     }
    //                     if (sendto(s, window[hit].data, fs_block_sz+1 , 0 , (struct sockaddr *) &si_other, slen) == -1)
    //                     {
    //                         fprintf(stderr, "ERROR: Failed to send file %s. \n", argv[argc-1]);
    //                         die("sendto()");
    //                     }
    //                     printf("Mengirim frame ke-%d : %s\n", hit, window[hit].data);
    //                 }
    //                 hit++;
    //                 sleep(1);
    //             }
    //             printf("Menunggu Acknowledgement\n");
    //             sleep(10);
                
    //             cek=1;
    //             hit=0;
    //             while(hit<counter){
    //                 if (window[hit].ackFlag==0)
    //                     cek=0;
    //                 hit++;
    //             }
    //         }
    //     }

    // }



    memset(buf,'\0', BUFLEN);
    printf("Send EOF\n");
    buf[0]=Endfile;
    buf[1]='\0';
    if (sendto(s, buf, 2 , 0 , (struct sockaddr *) &si_other, slen) == -1)
    {
        fprintf(stderr, "ERROR: Failed to send EOF character\n");
        die("sendto()");
    }

    fclose(fs);
    
    printf("Karakter selesai dikirim, menutup socket\n");
    
    isSocketOpen = 0;
    close(s);
    printf("Socket ditutup!\n");

    printf("Finish!\n");
    return 0;
}
