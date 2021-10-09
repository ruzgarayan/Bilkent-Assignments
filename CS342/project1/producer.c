#include <stdlib.h>
#include <stdio.h>
#include <time.h> 

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Wrong format. Use the arguments"
			"\n\tproducer <M>"
			"\n\n\t<M> number of characters to print\n");
		return 0;
	}

	int M = atoi(argv[1]);
	if (M < 0)
	{
		printf("Wrong arguments. M must be nonnegative\n");
		return 0;
	}

	srand(time(NULL));

	for (int i = 0; i < M; i++)
	{
		char randChar = (rand() % 96) + 32; 
		printf("%c", randChar);
	}

	return 0;
}
