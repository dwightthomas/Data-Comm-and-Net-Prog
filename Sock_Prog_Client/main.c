#include <sys/socket.h>       // socket definitions
#include <sys/types.h>        // socket types
#include <arpa/inet.h>        // inet (3) funtions
#include <unistd.h>           // misc. UNIX functions
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global constants
#define LISTENQ        (1024)   // Backlog for listen()

int main(int argc, char *argv[])
{
    //Socket Variables
    int       conn_s;                // connection socket
    short int port;                  // port number
    struct    sockaddr_in servaddr;  // socket address structure
    char     *bufferr;
    char     *szAddress;             // Holds remote IP address
    char     *szPort;                // Holds remote port
    char     *endptr;                // for strtol()

    // Variables to be pasted to server
    char *file_path;
    char *format;
    char *target;


    // Get command line arguments

    if(argc == 6)
    {
        szAddress = argv[1];
        szPort = argv[2];
        file_path = argv[3];
        format = argv[4];
        target = argv[5];
    }
    else
    {
        printf("Invalid amount of arguments.\nGood bye!!");
        return 0;
    }

     // Set the remote port

    port = strtol(szPort, &endptr, 0);
    if ( *endptr )
    {
        printf("ECHOCLNT: Invalid port supplied.\n");
        exit(EXIT_FAILURE);
    }

    // Create the listening socket

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    //  Set all bytes in socket address structure to
    // zero, and fill in the relevant data members

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);

    // Set the remote IP address

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 )
    {
        printf("ECHOCLNT: Invalid remote IP address.\n");
        exit(EXIT_FAILURE);
    }

    // connect() to the remote echo server

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 )
    {
        printf("ECHOCLNT: Error calling connect()\n");
        exit(EXIT_FAILURE);
    }

    // Setting data to be sent as request for the server

    strcpy(bufferr, file_path);
    strcat(bufferr, ",");
    strcat(bufferr, format);
    strcat(bufferr, ",");
    strcat(bufferr, target);
    strcat(bufferr, ",");

    //  Send string that holds the file path, format and target name.

    write(conn_s, bufferr, strlen(bufferr));
    read(conn_s, bufferr, 900);

    //  Output echoed string

    if(bufferr[0] == 'e' && bufferr[1] == 'r')
    {
        printf("\nFormat Error.\n");
    }
    else if(bufferr[0] == 's' && bufferr[1] == 'u')
    {
        printf("\nSuccess.\n");
    }

    // Close the connected socket

    if ( close(conn_s) < 0 )
    {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
