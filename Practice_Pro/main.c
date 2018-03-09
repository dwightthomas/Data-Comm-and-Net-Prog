#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int binaryToDecimal(long long n);

int main(int argc, char *argv[])
{
    FILE *input, *output;
	argc = 3;
	//Checks to make sure book input file location and output file location were pasted.
	if (argc != 3)
	{
		printf("You entered an invalid amount of arguments. \nError!\nGoodBye!!!\n");
		return 0;
	}

	//This opens both input and output files for reading and writing respectively.
    input = fopen(argv[1], "r");
    output = fopen(argv[2], "w");
    //This checks to ensure both file were opened properly
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

    //This is the while loop for reading the data.
    char* endline;
    while(endline != EOF)
    {
        long long amt;
        int type = 0, n = 0;
        fscanf(input, "%d", &type);
        //This prints the type to file (since right now all is type one just do it manually).
        fprintf(output, "00000001 ");

        //This determines the types and what to do with it.
        if(type == 0)
        {
            //Converts type 0 to type 1 and prints to the screen and writes to output file as well.
            long long number;
            int num = 0;
            fscanf(input, "%lld", &amt);
            n = binaryToDecimal(amt);
            if(n < 10)
            {
                //Print 2 zeros before it
                printf("00%d ", n);
                fprintf(output, "00%d ", n);
            }
            else if(n < 100)
            {
                //Print 1 zero before it.
                printf("0%d ", n);
                fprintf(output, "0%d ", n);
            }
            else
            {
                //Print it as normal
                printf("%d ", n);
                fprintf(output, "%d ", n);
            }
            int i;
            //Loop through n numbers
            for(i=0; i<n; i++)
            {
                fscanf(input, "%lld", &number);
                num = binaryToDecimal(number);

                if(i != n-1)
                {
                    printf("%d, ", num);
                    fprintf(output, "%d, ", num);
                }
                else
                {
                    printf("%d ", num);
                    fprintf(output, "%d ", num);
                }
            }
            //Use to move to next line an check for EOF
            printf("\n");
            fprintf(output, "\n");
            endline = fgetc(input);
        }
        //This is taken if it is for type 1
        else
        {
            int number;
            char comma;
            fscanf(input, "%lld", &amt);
            n = binaryToDecimal(amt);
            if(n < 10)
            {
                //Print 2 zeros before it
                printf("00%d ", n);
                fprintf(output, "00%d ", n);
            }
            else if(n < 100)
            {
                //Print 1 zero before it.
                printf("0%d ", n);
                fprintf(output, "0%d ", n);
            }
            else
            {
                //Print it as normal
                printf("%d ", n);
                fprintf(output, "%d ", n);
            }
            int i;
            for(i=0; i<n; i++)
            {
                if(i != n-1)
                {
                    fscanf(input, "%d%c", &number, &comma);
                    printf("%d, ", number);
                    fprintf(output, "%d, ", number);
                }
                else
                {
                    fscanf(input, "%d%c", &number, &comma);
                    printf("%d", number);
                    fprintf(output, "%d", number);
                }
            }
            //Use to move to next line an check for EOF
            printf("\n");
            fprintf(output, "\n");
            endline = fgetc(input);
        }
    }


    return 0;
}


int binaryToDecimal(long long n)
{
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
