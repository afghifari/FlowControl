/*
    Simple udp server
*/
#include "server.h" 

struct sockaddr_in si_me, si_other;
int s, slen = sizeof(si_other) , recv_len;
Byte status = XON;          // XON atau XOFF
unsigned send_xon = 0,
send_xoff = 0;

static int indexBuffer = 0;
static int front = 0;
static int rear = 0;

void die(char *s)
{
    perror(s);
    exit(1);
}
 
void *childRProcess(void *threadid) {
    static int counter = 1;
    char b[1];

    while (1) {

        //Hanya mengonsumsi buffer jika tidak 0
        if (indexBuffer > 0) {

            printf("Mengkonsumsi byte ke-%d : %s\n", counter++, bufer[front].data);
            memset(bufer[front].data,'\0', BUFLEN);
            // bzero(bufer[front].data, BUFLEN);
            front++;
            if(front == 8)
                front=0;
            indexBuffer--;
        }


        if (indexBuffer < MAX_LOWERLIMIT && status == XOFF) {
            printf("Buffer < maximum lowerlimit\n");
            printf("Mengirim XON\n");
            send_xon = 1; send_xoff = 0;

            b[0] = status = XON;
            if(sendto(s, b, 1, 0, (struct sockaddr *) &si_other, slen) < 0)
                error("ERROR: Gagal mengirim XON.\n");
        }   



        sleep(3);
    }
    pthread_exit(NULL);
} 

int main(int argc, char *argv[]) {
    pthread_t thread[1];
    int i;
    char b[1];
    char buf[BUFLEN];
    static int counter = 1;


    if (argc<2) {
        // argumen pemanggilan program kurang
        printf("Usage: %s <port>\n", argv[0]);
        return 0;
    }

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    
    printf("Membuat socket lokal pada port %s...\n", argv[1]); 
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(atoi(argv[1]));
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }


    // membuat child
    if(pthread_create(&thread[0], NULL, childRProcess, 0)) 
        error("ERROR: Gagal membuat proses anak");

     
    //keep listening for data
    // while(buf[0]!=Endfile)
    while(1)
    {
        fflush(stdout);
        bzero(buf, BUFLEN);
        // memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        
        //print details of the client/peer and the data received
        // printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Menerima paket ke-%d : %s\n", counter++, buf);
        
        //input data ke buffer
        if (indexBuffer < BUFLEN) {
            rear = (indexBuffer > 0) ? (rear+1) % 8 : rear;
            memset(bufer[rear].data,'\0', BUFLEN);
            // bzero(bufer[rear].data, BUFLEN);
            for (i=0;i<BUFLEN;i++) {
                bufer[rear].data[i] = '\0';
            }
            strncpy(bufer[rear].data,buf,BUFLEN);
            bufer[rear].dummy=12;
            //printf("dataaaaaaaaaaaaaaaaa : %s\n",bufer[rear].data);
            bufer[rear].data[BUFLEN]='\0';
            indexBuffer++;
        }

        // mengirim sinyal XOFF jika buffer mencapai Minimum Upperlimit
        if(indexBuffer >= (MIN_UPPERLIMIT) && status == XON) {
            printf("Buffer > minimum upperlimit\n");
            printf("Mengirim XOFF.\n");
            send_xoff = 1; send_xon = 0;
            b[0] = status = XOFF;

            if(sendto(s, b, 1, 0,(struct sockaddr *) &si_other, slen) < 0)
                error("ERROR: Gagal mengirim XOFF.\n");
        }

         
        //now reply the client with the same data
        // if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        // {
        //     die("sendto()");
        // }
    }
    
    printf("Data received\n");
    printf("Now close Connections\n");

    close(s);
    return 0;
}