#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Wrong format. Use the arguments"
			"\n\tconsumer <M>"
			"\n\n\t<M> number of characters to read\n");
		return 0;
	}

	int M = atoi(argv[1]);
	if (M < 0)
	{
		printf("Wrong arguments. M must be nonnegative\n");
		return 0;
	}

	for (int i = 0; i < M; i++)
	{
		getchar();
	}
}
