#include <sys/socket.h>       // socket definitions
#include <sys/types.h>        // socket types
#include <arpa/inet.h>        // inet (3) funtions
#include <unistd.h>           // misc. UNIX functions
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// Function declarations

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);

// Global constants
#define MAX_LINE           (1000)
#define LISTENQ        (1024)   // Backlog for listen()

int main(int argc, char *argv[])
{
    //Socket Variables
    int       conn_s;                // connection socket
    short int port;                  // port number
    struct    sockaddr_in servaddr;  // socket address structure
    char      buffer[MAX_LINE];      // character buffer
    char *bufferr;
    char     *szAddress;             // Holds remote IP address
    char     *szPort;                // Holds remote port
    char     *endptr;                // for strtol()

    //
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

    // Get string to echo from user

    /*printf("Enter the string to echo: ");
    fgets(bufferr, MAX_LINE, stdin);*/

    printf("The new buffer is: %s\n", bufferr);
    strcpy(bufferr, file_path);
    strcat(bufferr, ",");
    strcat(bufferr, format);
    strcat(bufferr, ",");
    strcat(bufferr, target);
    strcat(bufferr, ",");
    printf("\nThe new buffer now is: %s\n", bufferr);


    //  Send string that holds the file path, format and target name.

    write(conn_s, bufferr, strlen(bufferr));
    read(conn_s, bufferr, 900);
    //read(conn_s, bufferr, Max-1);


    //  Output echoed string

    printf("Echo response: %5s\n", bufferr);

    // Close the connected socket

        if ( close(conn_s) < 0 )
        {
            fprintf(stderr, "ECHOSERV: Error calling close()\n");
            exit(EXIT_FAILURE);
        }

    return EXIT_SUCCESS;
}

// Read a line from a socket

ssize_t Readline(int sockd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ )
    {

        if ( (rc = read(sockd, &c, 1)) == 1 )
        {
            *buffer++ = c;
            if ( c == '\n' )
                break;
        }
        else if ( rc == 0 )
        {
            if ( n == 1 )
                return 0;
            else
                break;
        }
        else
        {
            if ( errno == EINTR )
                continue;
            return -1;
        }
    }

    *buffer = 0;
    return n;
}


//  Write a line to a socket

ssize_t Writeline(int sockd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 )
    {
        if ( (nwritten = write(sockd, buffer, nleft)) <= 0 )
        {
            if ( errno == EINTR )
                nwritten = 0;
            else
                return -1;
        }
        nleft  -= nwritten;
        buffer += nwritten;
    }

    return n;
}





