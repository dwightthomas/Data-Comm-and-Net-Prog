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

typedef int bool;
#define true 1
#define false 0

// Helper functions

int ascii_to_int(unsigned long long num);
int binaryToDecimal(long long n);
unsigned long long int_to_ascii(int num);
int amount_ascii(int n);
unsigned long long decimalToBinary(int n);
void print_binary_leading_zeros(unsigned long long n, bool isAmt);
int type0_to_type1(char* line);
int typee1_to_type0(char* line);
int read_type(char *line);


FILE *output;

int main(int argc, char *argv[])
{
    int sock;                // listening socket
    struct    sockaddr_in myaddr;  // socket address structure for server
    struct sockaddr_storage cliaddr;  // socket address structure for client
    //char *ack = "ack";
    socklen_t clilen;        // Client socket length
    int msglen;              // Message length
    short int port;                  // port number
    char     *endptr;                // for strtol()
    float loss_ratio;
    int rseed;
    int Maxlen= 2000;

    char *output_file_path;
    char *output_Name;
    char *error_msg = "error";
    int format;

    // Get port number from the command line
    // if no arguments were supplied Print error message

    if ( argc == 4 )
    {
        port = strtol(argv[1], &endptr, 0);
        loss_ratio = atof(argv[2]);
        rseed = atoi(argv[3]);
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

    if((sock = socket (PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    memset((char*) &myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);  //This is my port
    myaddr.sin_addr.s_addr = htonl( INADDR_ANY );

    // Bind our socket addresss to the socket

    if(bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
    {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");

        exit(EXIT_FAILURE);
    }

    // Recieve Request from Client Program

    clilen = sizeof(cliaddr);
    while(1)
    {
        char msg[2000];
        Maxlen = 2000;
        msglen = recvfrom(sock, msg, Maxlen, 0, (struct sockaddr *)&cliaddr, &clilen);
        if(msglen < 0)
        {
            printf("Error occured recieveing the data\n");
            return 0;
        }
        ///Breaking msg into Repestive Variables
        char *response;
        response = strtok(msg, ",");
        output_file_path = response;
        response = strtok(NULL, ",");
        format = atoi(response);
        response = strtok(NULL, ",");
        output_Name = response;

        ///This is the Ack message for the stop and wait.
        ///This needs to be tested on Linux machine
        ///This is the ACK Didnt get to test it
        char ack[2000];
        sprintf(ack, "%s",  "ack");
        Maxlen = strlen(msg)+1;
        //msglen = lossy_sendto(loss_ratio, rseed, sock, ack, Maxlen, (struct sockaddr *)&cliaddr, clilen);

        ///Opening file
        output = fopen(output_Name, "w+");
        if(output == NULL)
        {
            printf("The output file could not be opened\nGood bye!");
            return 0;
        }


        ///Recieveing the data from file line by line.
        ///CHANGING FORMAT OF DATA
        int succes = 0;
        while(1)
        {
            char line[2000];
            Maxlen = 2000;
            ///Recieveing data from file line by line.
            msglen = recvfrom(sock, line, Maxlen, 0, (struct sockaddr *)&cliaddr, &clilen);
            if(msglen < 0)
            {
                printf("Error occured recieveing the data\n");
            }

            ///This is the Ack message for the stop and wait.
            ///This needs to be tested on Linux machine
            ///This is the ACK Didnt get to test it
            char ack[2000];
            sprintf(ack, "%s",  "ack");
            Maxlen = strlen(msg)+1;
            //msglen = lossy_sendto(loss_ratio, rseed, sock, ack, Maxlen, (struct sockaddr *)&cliaddr, clilen);


            ///For ending loop to Finish Recieving
            if(line[0] == 'e')
            {
                succes = 1;
                break;
            }


            ///HANDLING the Date in the FILE

             if(format == 0)
            {
                /// No translations so just reaad and write
                fprintf(output, "%s\n\n", line);
            }
            else if(format == 1)
            {
                int type = read_type(line);
                if(type == 1)
                {
                    /// Leaving type1 as type1
                    fprintf(output, "%s", line);
                }
                /// Format says change all type0 to type1
                else if(type == 0)
                {
                    /// Changing type0 to type1

                    /// This function is used tto convert type zero units to type one
                    int res = type0_to_type1(line);
                    if(res == -1)
                    {
                        /// Wrong format wrtie error message
                       /// This closes the file and opens it again to wipe anything that was previously saved.
                        fclose(output);
                        ///This is to send response from Server
                        ///For Errors
                        char msg[2000];
                        sprintf(msg, "%s",  "error");
                        Maxlen = strlen(msg)+1;
                        msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                    }
                }
                else
                {
                    /// Wrong format wrtie error message
                   /// This closes the file and opens it again to wipe anything that was previously saved.
                    fclose(output);
                    ///This is to send response from Server
                    ///For Errors
                    char msg[2000];
                    sprintf(msg, "%s",  "error");
                    Maxlen = strlen(msg)+1;
                    msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                }
            }
            else if(format == 2)
            {
            /// Format says change all type1 to type0
                int type = read_type(line);
                if(type == 0)
                {
                    /// Leave type0 as type0
                    fprintf(output, "%s", line);
                }
                else if(type == 1)
                {
                    /// Changing type1 to type0

                    int res = typee1_to_type0(line);
                    if(res == -1)
                    {
                        /// Wrong format wrtie error message
                       /// This closes the file and opens it again to wipe anything that was previously saved.
                        fclose(output);
                        ///This is to send response from Server
                        ///For Errors
                        char msg[2000];
                        sprintf(msg, "%s",  "error");
                        Maxlen = strlen(msg)+1;
                        msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                    }
                }
                else
                {
                    /// Wrong format wrtie error message
                   /// This closes the file and opens it again to wipe anything that was previously saved.
                    fclose(output);
                    ///This is to send response from Server
                    ///For Errors
                    char msg[2000];
                    sprintf(msg, "%s",  "error");
                    Maxlen = strlen(msg)+1;
                    msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                }
            }
            else if(format == 3)
            {
            /// Format for changing both types to the other
                int type = read_type(line);
                if(type == 0)
                {
                    /// Changing type0 to type1
                    int res = type0_to_type1(line);
                    if(res == -1)
                    {
                        /// Wrong format wrtie error message
                       /// This closes the file and opens it again to wipe anything that was previously saved.
                        fclose(output);
                        ///This is to send response from Server
                        ///For Errors
                        char msg[2000];
                        sprintf(msg, "%s",  "error");
                        Maxlen = strlen(msg)+1;
                        msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                    }
                }
                else if(type == 1)
                {
                    /// Changing type1 to type0
                    int res = typee1_to_type0(line);
                    if(res == -1)
                    {
                        /// Wrong format wrtie error message
                       /// This closes the file and opens it again to wipe anything that was previously saved.
                        fclose(output);
                        ///This is to send response from Server
                        ///For Errors
                        char msg[2000];
                        sprintf(msg, "%s",  "error");
                        Maxlen = strlen(msg)+1;
                        msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                    }
                }
                else
                {
                    /// Wrong format wrtie error message
                   /// This closes the file and opens it again to wipe anything that was previously saved.
                    fclose(output);
                    ///This is to send response from Server
                    ///For Errors
                    char msg[2000];
                    sprintf(msg, "%s",  "error");
                    Maxlen = strlen(msg)+1;
                    msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
                }
        }
            else
            {
                /// Wrong format wrtie error message
               /// This closes the file and opens it again to wipe anything that was previously saved.
                fclose(output);
                ///This is to send response from Server
                ///For Errors
                char msg[2000];
                sprintf(msg, "%s",  "error");
                Maxlen = strlen(msg)+1;
                msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
            }

            fclose(output);
        }

        ///Succes Fulling Recieved and converted Data
        if(succes == 1)
        {
            ///This is to send response from Server
            char msg[2000];
            sprintf(msg, "%s",  "sucess");
            Maxlen = strlen(msg)+1;
            msglen = lossy_sendto(loss_ratio, rseed, sock, msg, Maxlen, (struct sockaddr *)&cliaddr, clilen);
        }

        ///Close socket and End
        close(sock);
        break;
    }

}

//CHANGE ALL HELPER FUNCTIONS TO HANDLE NEW FORMAT

int ascii_to_int(unsigned long long num)
{
    /// This function converts ascii characters to integers
    int i = 1, sum = 0;
    while(num != 0)
    {
        int temp = num%100;
        if(temp < 48)
        {
            return -1;
        }
        temp = temp - 48;
        sum = sum + (temp*i);
        i = i*10;
        num = num/100;
    }
    return sum;
}

int binaryToDecimal(long long n)
{
    /// This function converts binary number to decimal
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        if(remainder < 0 || remainder > 1)
        {
            return -1;
        }
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

unsigned long long int_to_ascii(int num)
{
    /// This converts integers to ascii values
    unsigned long long i = 1, sum = 0;
    if(num == 0)
        return 48;
    while(num != 0)
    {
        /// Fix the sum type to unsigned long long
        int temp = num%10;
        temp = temp + 48;
        sum = sum + (i*temp);
        i = i * 100;
        num = num/10;
    }
    return sum;
}

int amount_ascii(int n)
{
    /// This functions ensure that the amount variable has has 3 bytes
    if(n<100)
    {
        n = n + 484800;
    }
    else
    {
        n = n + 480000;
    }
    return n;
}

unsigned long long decimalToBinary(int n)
{
    /// This function converts Decimal to binary numbers
    unsigned long long binaryNumber = 0, i = 1;
    int remainder, step = 1;
    while (n!=0)
    {
        remainder = n%2;
        n /= 2;
        binaryNumber += remainder*i;
        i *= 10;
    }
    return binaryNumber;
}

void print_binary_leading_zeros(unsigned long long n, bool isAmt)
{
    /// This function is responsible for putting all the leading zeros in for binary numbers
    if(n < 10)
    {
        if(isAmt == true)
        {
            fprintf(output, "0000000%lld ", n);
        }
        else
        {
            fprintf(output, "000000000000000%lld ", n);
        }
    }
    else if(n < 100)
    {
        if(isAmt == true)
        {
            fprintf(output, "000000%lld ", n);
        }
        else
        {
            fprintf(output, "00000000000000%lld ", n);
        }
    }
    else if(n < 1000)
    {
        if(isAmt == true)
        {
            fprintf(output, "00000%lld ", n);
        }
        else
        {
            fprintf(output, "0000000000000%lld ", n);
        }
    }
    else if(n < 10000)
    {
        if(isAmt == true)
        {
            fprintf(output, "0000%lld ", n);
        }
        else
        {
            fprintf(output, "000000000000%lld ", n);
        }
    }
    else if(n < 100000)
    {
        if(isAmt == true)
        {
            fprintf(output, "000%lld ", n);
        }
        else
        {
            fprintf(output, "00000000000%lld ", n);
        }
    }
    else if(n < 1000000)
    {
        if(isAmt == true)
        {
            fprintf(output, "00%lld ", n);
        }
        else
        {
            fprintf(output, "0000000000%lld ", n);
        }
    }
    else if(n < 10000000)
    {
        if(isAmt == true)
        {
            fprintf(output, "0%lld ", n);
        }
        else
        {
            fprintf(output, "000000000%lld ", n);
        }
    }
    else if(n < 100000000)
    {
        if(isAmt == true)
        {
            fprintf(output, "%lld ", n);
        }
        else
        {
            fprintf(output, "00000000%lld ", n);
        }
    }
    else if(n < 1000000000)
    {
        fprintf(output, "0000000%lld ", n);
    }
    else if(n < 10000000000)
    {
        fprintf(output, "000000%lld ", n);
    }
    else if(n < 100000000000)
    {
        fprintf(output, "00000%lld ", n);
    }
    else if(n < 1000000000000)
    {
        fprintf(output, "0000%lld ", n);
    }
    else if(n < 10000000000000)
    {
        fprintf(output, "000%lld ", n);
    }
    else if(n < 100000000000000)
    {
        fprintf(output, "00%lld ", n);
    }
    else if(n < 1000000000000000)
    {
        fprintf(output, "0%lld ", n);
    }
    else if(n < 10000000000000000)
    {;
        fprintf(output, "%lld ", n);
    }
}

int type0_to_type1(char *line)
{
    /// This function is used tto convert type zero units to type one
    unsigned long long amt, number;
    int n=0;
    char *response;
    response = strtok(line, " ");
    int type =  atoi(response);
    fprintf(output, "00000001 ");
    response = strtok(NULL, " ");
    amt = atoll(response);
    n = binaryToDecimal(amt);
    amt = int_to_ascii(n);
    amt = amount_ascii(amt);
    fprintf(output, "%d ", amt);
    int i;
    for(i=0; i<n; i++)
    {
        response = strtok(NULL, " ");
        number = atoll(response);
        number = binaryToDecimal(number);
        if(number == -1)
        {
            return -1;
        }
        number = int_to_ascii(number);
        if(i != n-1)
        {
            fprintf(output, "%lld, ", number);
        }
        else
        {
            fprintf(output, "%lld\r\n", number);
        }
    }
    return 0;
}

int typee1_to_type0(char* line)
{
    /// This function convert type one to type zero
    fprintf(output, "00000000 ");
    int amt;
    char *response;
    response = strtok(line, " ");
    int type =  atoi(response);
    response = strtok(NULL, " ");
    amt = atoi(response);
    amt = ascii_to_int(amt);
    unsigned long long bin;
    bin = decimalToBinary(amt);
    print_binary_leading_zeros(bin, true);
    int i;
    for(i=0; i<amt; i++)
    {
        unsigned long long number = 0;
        char comma;
        if(i == amt-1)
            response = strtok(NULL, " ");
        else
            response = strtok(NULL, ", ");
        number = atoll(response);
        number = ascii_to_int(number);
        if(number == -1)
        {
            return -1;
        }
        bin = decimalToBinary(number);
        print_binary_leading_zeros(bin, false);
    }
    fprintf(output, "\r\n");
    return 0;
}

int read_type(char* line)
{
    /// This function is used to read in the type from the units in file
    int type;
    type = line[7] - '0';
    return type;
}
