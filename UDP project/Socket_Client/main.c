#include <sys/socket.h>       // socket definitions
#include <sys/types.h>        // socket types
#include <arpa/inet.h>        // inet (3) funtions
#include <unistd.h>           // misc. UNIX functions
#include <errno.h>
#include "sendlib.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>

#define true 1
#define false 0
#define timeout_time 5

// Helper Functions
ssize_t custom_recvfrom(int sockfd, const void *buff, size_t nbytes, int flags, struct sockaddr* from, socklen_t *fromaddrlen);
void handle_alarm();

FILE *input;

int main(int argc, char *argv[])
{
    int sock;                // listening socket
    short int port;                  // port number
    struct    sockaddr_in myaddr;  // socket address structure for server
    struct sockaddr_storage serStorage;               // message to recieve.
    socklen_t serlen, storagelen;        // Server socket length
    int msglen;              // Message length
    char     *szAddress;             // Holds remote IP address
    char     *szPort;                // Holds remote port
    char     *endptr;                // for strtol()
    int MaxMsgLen = 2000;

    // Variables to be pasted to server
    char *file_path;
    char *format;
    char *target;
    char *loss_prob;
    char *rseed;

    // Get command line arguments

    if(argc == 8)
    {
        szAddress = argv[1];
        szPort = argv[2];
        file_path = argv[3];
        format = argv[4];
        target = argv[5];
        loss_prob = argv[6];
        rseed = argv[7];
    }
    else
    {
        printf("Invalid amount of arguments.\nGood bye!!");
        return 0;
    }

    // Conversion of variable to correct format
    float lossR = atof(loss_prob);
    int seedR = atoi(rseed);


    port = strtol(szPort, &endptr, 0);
    if ( *endptr )
    {
        printf("ECHOCLNT: Invalid port supplied.\n");
        exit(EXIT_FAILURE);
    }

    // Create the listening socket

    if((sock = socket (PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    memset((char*) &myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);  //This is my port
    myaddr.sin_addr.s_addr = inet_addr(szAddress);  // This is my IP

    // Setting the socket length base off of the socket address.

    serlen = sizeof(myaddr);
    storagelen = sizeof(serStorage);


    // Send Request to Server Program

    while(1)
    {
        /// Sending the First paramenters to the Server
        ///Stop and wait protocal being implemented.
        ///Needs To be done on Linux.
        //msglen = lossy_sendto(lossR, seedR, sock, msg, 10, (struct sockaddr *)&myaddr, serlen);

        char msg[2000];
        sprintf(msg, "%s,%s,%s",  file_path,format,target);
        MaxMsgLen = strlen(msg)+1;


        //Used this to ensure everything else was working becasue I could not test Alarms on Window machince
        msglen = sendto(sock, msg, MaxMsgLen, 0, (struct sockaddr *)&myaddr, serlen);
        printf("The return of sento is %d\n", msglen);


        /// To Recieve Ack from Server
        ///For stop and wait will be tested on VM of linux hopefully.
        //int test = custom_recvfrom(sock, file_path, 20, 0, (struct sockaddr *)&serStorage, &storagelen);
        /*if(test == -5)
            printf("It timedout\n");
        if(test == -4)
            printf("Some other error occured\n");
        else
            printf("The returned msg is: %s\n", msg);*/


        /*int test = recvfrom(sock, ack, MaxMsgLen, 0, (struct sockaddr *)&serStorage, &storagelen);
        if(test < 0 )
            perror("Recieve error");*/




        ///Opening the File to send the data.
        input = fopen(file_path, "r");
        if(input == NULL)
        {
            printf("The input file could not be opened\nGood bye!");
            return 0;
        }

        ///Sending the data from File line by line.
        int maxline = 2000;
        char line[2000];
        char mess[2000];
        while(1)
        {
            if(fgets(line, sizeof(line), input) == NULL)
            {
                ///Sending end to Server so it know we are at end of file
                char end[] = "end";
                memset(mess,0,strlen(mess));
                sprintf(mess, "%s", end);
                maxline = strlen(mess) + 1;
                ///Used to let Server Know it is the end of File.
                msglen = sendto(sock, mess, maxline, 0, (struct sockaddr *)&myaddr, serlen);
                if(msglen < 0)
                    perror("Sending line");
                break;
            }

            ///Send line by line of the data in the file
            sprintf(mess, "%s",  line);
            maxline = strlen(mess) +1;
            ///Part of Stop and wait protocal being implemented.
            ///Needs To be done on Linux.
            //msglen = lossy_sendto(lossR, seedR, sock, line, 10, (struct sockaddr *)&myaddr, serlen);

            //Used this to ensure everything else was working becasue I could not test Alarms on Window machince
            msglen = sendto(sock, mess, maxline, 0, (struct sockaddr *)&myaddr, serlen);
            if(msglen < 0)
                perror("Sending line");
        }

        ///Handle the Last Response
        char ack[2000];
        MaxMsgLen = 2000;
        int test = recvfrom(sock, ack, MaxMsgLen, 0, (struct sockaddr *)&serStorage, &storagelen);
        if(test < 0 )
            perror("Recieve error");

        if(ack[0] == 'e' && ack[1] == 'r')
        {
            printf("\nFormat Error!\n");
        }
        else if(ack[0] == 's' && ack[1] == 'u')
        {
            printf("\nSucces!\n");
        }

        ///End of Sesion!
        break;

    }
}

ssize_t custom_recvfrom(int sockfd, const void *buff, size_t nbytes, int flags, struct sockaddr* from, socklen_t *fromaddrlen)
{
    volatile sig_atomic_t timeout = false;


    ssize_t n;

    /// This for Alarms and will test inside VM hopefully.
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = (void (*)(int))handle_alarm;
    action.sa_flags = SA_RESTART;
    if(sigaction(SIGALRM, &action, 0) == -1)
    {
        perror("sigaction");
        return 1;
    }

 //   signal(SIGALRM, &handle_alarm);
    int count =0;
    while(1)
    {
        alarm(5);
        printf("count is = %d\n", count);
        sleep(1);
    }

        /*n = recvfrom(sockfd, buff, 20, flags, NULL, NULL);
        printf("Do I get right after the recieve line\n");
        if (n < 0)
        {
            if (errno==EINTR)
                /* timed out
                return -5;
            else
            {
                /* some other error
                perror("The reciving on client error is");
                return -4;
            }
        }
        else
            /* no error or time out - turn off alarm
            alarm(0);*/

    return n;
}

void handle_alarm()
{
    printf("The Alarm Signal went!");
        //return;
}
