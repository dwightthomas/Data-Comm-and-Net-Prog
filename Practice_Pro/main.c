#include <stdio.h>
#include <stdlib.h>

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

    input = fopen("Test.txt", "r");
    if(input == NULL)
    {
        printf("The input file could not be opened\nGood bye!");
        return 0;
    }
    char line[100];
    int len = 100;
    printf("I get here\n");
    while(fgets(line, len, input) != NULL)
    {
        printf("The line is: %s\n", line);
    }


    return 0;
}
