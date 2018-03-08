#include <sys/socket.h>       // socket definitions
#include <sys/types.h>        // socket types
#include <arpa/inet.h>        // inet (3) funtions
#include <unistd.h>           // misc. UNIX functions
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



// Global constants

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)
#define LISTENQ        (1024)   /*  Backlog for listen()   */


int main(int argc, char *argv[])
{
    int       list_s;                // listening socket
    int       conn_s;                // connection socket
    short int port;                  // port number
    struct    sockaddr_in servaddr;  // socket address structure
    char      buffer[MAX_LINE];      // character buffer
    char      *bufferr;
    size_t    Max =1000;
    char     *endptr;                // for strtol()

    char *input_path;
    char *output_path;
    int format;

    // Get port number from the command line
    // if no arguments were supplied Print error message

    if ( argc == 2 )
    {
        port = strtol(argv[1], &endptr, 0);
        if ( *endptr )
        {
            fprintf(stderr, "ECHOSERV: Invalid port number.\n");
            exit(EXIT_FAILURE);
        }
    }
    else if ( argc < 2 )
    {
        fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
        exit(EXIT_FAILURE);
    }

    // Create the listening socket

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    // Set all bytes in socket address structure to
    // zero, and fill in the relevant data members

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);

    // Bind our socket addresss to the
	// listening socket, and call listen()

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
    {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 )
    {
        fprintf(stderr, "ECHOSERV: Error calling listen()\n");
        exit(EXIT_FAILURE);
    }

    //This is where the real work happens

    // Enter an infinite loop to respond
    // to client requests and echo input

    while ( 1 )
    {
        // Wait for a connection, then accept() it  */

        if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
        {
            fprintf(stderr, "ECHOSERV: Error calling accept()\n");
            exit(EXIT_FAILURE);
        }


        // Retrieve an input line from the connected socket
        // then simply write it back to the same socket.


        read(conn_s, bufferr, Max-1);



        // This is the process of taking input and putting everything where it needs to go
        char *response;
        response = strtok(bufferr, ",");
        input_path = response;
        printf("The input is: %s\n", input_path);
        response = strtok(NULL, ",");
        format = atoi(response);
        printf("The format is: %d\n", format);
        response = strtok(NULL, ",");
        output_path = response;
        printf("The output is: %s\n", output_path);

        //From here I have to check to seee if i can read from the file over the socket.

        FILE *input, *output;
        input = fopen(input_path, "r");
        if(input == NULL)
        {
            printf("The input file could not be opened\nGood bye!");
            return 0;
        }

        int type = 0, n = 0;
        fscanf(input, "%d", &type);
        printf("The typ is: %d\n", type);






        // Close the connected socket

        if ( close(conn_s) < 0 )
        {
            fprintf(stderr, "ECHOSERV: Error calling close()\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
