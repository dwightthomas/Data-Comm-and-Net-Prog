#include <sys/socket.h>       // socket definitions
#include <sys/types.h>        // socket types
#include <arpa/inet.h>        // inet (3) funtions
#include <unistd.h>           // misc. UNIX functions
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


// Global constants

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)
#define LISTENQ        (1024)   /*  Backlog for listen()   */
typedef int bool;
#define true 1
#define false 0
FILE *input, *output;

// Helper functions

int ascii_to_int(int num);
int binaryToDecimal(long long n);
int int_to_ascii(int num);
int amount_ascii(int n);
long long decimalToBinary(int n);
void print_binary_leading_zeros(long long n, bool isAmt);
void type0_to_type1();
void typee1_to_type0();
int read_type();




int main(int argc, char *argv[])
{
    int       list_s;                // listening socket
    int       conn_s;                // connection socket
    short int port;                  // port number
    struct    sockaddr_in servaddr;  // socket address structure
    char      *bufferr;
    size_t    Max =1000;
    char     *endptr;                // for strtol()

    char *input_path;
    char *output_path;
    char *error_msg = "error";
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

    /// This is where the real work happens

    /// Enter an infinite loop to respond
    /// to client requests and echo input

    while ( 1 )
    {
        /// Wait for a connection, then accept() it

        if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
        {
            fprintf(stderr, "ECHOSERV: Error calling accept()\n");
            exit(EXIT_FAILURE);
        }


        /// Retrieve an input line from the connected socket

        read(conn_s, bufferr, Max-1);

        /// This is the process of taking input and putting everything where it needs to go

        char *response;
        response = strtok(bufferr, ",");
        input_path = response;
        response = strtok(NULL, ",");
        format = atoi(response);
        response = strtok(NULL, ",");
        output_path = response;


        /// Opens the files to for reading and writing

        input = fopen(input_path, "r");
        output = fopen(output_path, "w+");
        if(input == NULL)
        {
            printf("The input file could not be opened\nGood bye!");
            return 0;
        }
        if(output == NULL)
        {
            printf("The output file could not be opened\nGood bye!");
            return 0;
        }



        /// Handle the format dealing with which type of transformation needs to occur

        int maxline = 2000;
        char line[2000];
        bool isSucessful = true;
        if(format == 0)
        {
            /// No translations so just reaad and write
            while(1)
            {
                if(fgets(line, maxline, input) == NULL)
                    break;
                fprintf(output, "%s", line);
            }
            /// Sending the success message after making it out loop
            /// Wont change the error message sent if error was found because their is only one read on the client end
            /// Meaning it would only get the first message sent
            char *succes_msg = "success";
            write(conn_s, succes_msg, strlen(succes_msg));
        }
        else if(format == 1)
        {
            /// Format says change all type0 to type1
            char endline;
            while(endline != EOF)
            {
                int type = read_type();
                if(type == 1)
                {
                    /// Leaving type1 as type1
                    fgets(line, maxline, input);
                    fprintf(output, "0000000%d%s", type, line);
                }
                else if(type == 0)
                {
                    /// Changing type0 to type1
                    type0_to_type1();
                }
                else
                {
                    /// Wrong format wrtie error message
                    /// This closes the file and opens it again to wipe anything that was previously saved.
                    write(conn_s, error_msg, strlen(error_msg));
                    fclose(output);
                    output = fopen(output_path, "w");
                    isSucessful = false;
                    break;
                }
                endline = fgetc(input);
            }
            /// Sending the success message after making it out loop
            /// Wont change the error message sent if error was found because their is only one read on the client end
            /// Meaning it would only get the first message sent
            char *succes_msg = "success";
            write(conn_s, succes_msg, strlen(succes_msg));
        }
        else if(format == 2)
        {
            /// Format says change all type1 to type0
            char endline;
            while(endline != EOF)
            {
                int type = read_type();
                if(type == 0)
                {
                    /// Leave type0 as type0
                    fgets(line, maxline, input);
                    fprintf(output, "0000000%d%s", type, line);
                }
                else if(type == 1)
                {
                    /// Changing type1 to type0
                    typee1_to_type0();
                }
                else
                {
                    /// Wrong format wrtie error message
                    /// This closes the file and opens it again to wipe anything that was previously saved.
                    write(conn_s, error_msg, strlen(error_msg));
                    fclose(output);
                    output = fopen(output_path, "w");
                    isSucessful = false;
                    break;
                }
                endline = fgetc(input);
            }
            /// Sending the success message after making it out loop
            /// Wont change the error message sent if error was found because their is only one read on the client end
            /// Meaning it would only get the first message sent
            char *succes_msg = "success";
            write(conn_s, succes_msg, strlen(succes_msg));
        }
        else if(format == 3)
        {
            /// Format for changing both types to the other
            char endline;
            while(endline != EOF)
            {
                int type = read_type();
                if(type == 0)
                {
                    /// Changing type0 to type1
                    type0_to_type1();
                }
                else if(type == 1)
                {
                    /// Changing type1 to type0
                    typee1_to_type0();
                }
                else
                {
                    /// Wrong format wrtie error message
                    /// This closes the file and opens it again to wipe anything that was previously saved.
                    write(conn_s, error_msg, strlen(error_msg));
                    fclose(output);
                    output = fopen(output_path, "w");
                    isSucessful = false;
                    break;
                }
                endline = fgetc(input);
            }
            /// Sending the success message after making it out loop
            /// Wont change the error message sent if error was found because their is only one read on the client end
            /// Meaning it would only get the first message sent
            char *succes_msg = "success";
            write(conn_s, succes_msg, strlen(succes_msg));
        }
        else
        {
            /// Wrong format wrtie error message
           /// This closes the file and opens it again to wipe anything that was previously saved.
            write(conn_s, error_msg, strlen(error_msg));
            fclose(output);
            output = fopen(output_path, "w");
            isSucessful = false;
        }

        fclose(input);
        fclose(output);

        /// Close the connected socket

        if ( close(conn_s) < 0 )
        {
            fprintf(stderr, "ECHOSERV: Error calling close()\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            /// This is to end loop as the function as my protocol only allows one request at a time.
            break;
        }
    }

    return 0;
}

int ascii_to_int(int num)
{
    /// This function converts ascii characters to integers
    int i = 1, sum = 0;
    while(num != 0)
    {
        int temp = num%100;
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
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

int int_to_ascii(int num)
{
    /// This converts integers to ascii values
    int sum =0;
    unsigned long long temps = 0, i = 1;
    while(num != 0)
    {
        /// Fix the sum type to unsigned long long
        int temp = num%10;
        temp = temp + 48;
        sum = sum + (i*temp);
        temps = temps + (i*temp);
        printf("The sum is %d\n", sum);
        printf("the holder is %lld\n", temps);
        printf("I is %llu\n", i);
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

long long decimalToBinary(int n)
{
    /// This function converts Decimal to binary numbers
    long long binaryNumber = 0;
    int remainder, i = 1, step = 1;
    while (n!=0)
    {
        remainder = n%2;
        n /= 2;
        binaryNumber += remainder*i;
        i *= 10;
    }
    return binaryNumber;
}

void print_binary_leading_zeros(long long n, bool isAmt)
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

void type0_to_type1()
{
    /// This function is used tto convert type zero units to type one
    long long amt, number;
    int n=0;
    fprintf(output, "00000001 ");
    fscanf(input, "%lld", &amt);
    n = binaryToDecimal(amt);
    amt = int_to_ascii(n);
    amt = amount_ascii(amt);
    fprintf(output, "%d ", amt);
    int i;
    for(i=0; i<n; i++)
    {
        fscanf(input, "%ldd", &number);
        number = binaryToDecimal(number);
        number = int_to_ascii(number);
        if(i != n-1)
        {
            fprintf(output, "%d, ", number);
        }
        else
        {
            fprintf(output, "%d\r\n", number);
        }
    }
}

void typee1_to_type0()
{
    /// This function convert type one to type zero
    fprintf(output, "00000000 ");
    int amt;
    fscanf(input, "%d", &amt);
    amt = ascii_to_int(amt);
    long long bin;
    bin = decimalToBinary(amt);
    print_binary_leading_zeros(bin, true);
    int i;
    for(i=0; i<amt; i++)
    {
        int number=0;
        char comma;
        fscanf(input, "%d%c", &number, &comma);
        number = ascii_to_int(number);
        bin = decimalToBinary(number);
        print_binary_leading_zeros(bin, false);
    }
    fprintf(output, "\r\n");
}

int read_type()
{
    /// This function is used to read in the type from the units in file
    int type = 0;
    fscanf(input, "%d", &type);
    return type;
}
