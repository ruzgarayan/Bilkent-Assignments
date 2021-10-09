#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

const int MAX_COMMAND_SIZE = 256;
const int MAX_ARGUMENTS = 128;

//Read and return a line from stdin until a '\n' is seen
char* ispReadLine()
{
	int commandSize = MAX_COMMAND_SIZE;
	char* buf = malloc(sizeof(char) * commandSize);

	int i = 0;
	char nextChar;
	while(i < commandSize)
	{
		nextChar = getchar();
		if (nextChar == '\n')
		{
			buf[i] = '\0'; //End the string;
			break;
		}

		buf[i++] = nextChar;
	}
	if (i >= commandSize)
	{
		printf("The command entered should be at most %d characters long.\nExtra characters are not considered.\n"
			, MAX_COMMAND_SIZE);
	}

	return buf;
}

//Given the string for the current line,
//breaks it into an array arguments
//using the spaces in between the arguments
//Returns the number of arguments in the array
//Adds a NULL to the first non-used array index
int ispParseLine(char* line, char*** arguments)
{
	int argumentSize = MAX_ARGUMENTS;
	char** buf = malloc(sizeof(char *) * argumentSize);

	int i = 0;
	char* nextArgument;
	nextArgument = strtok(line, " ");
	while (nextArgument != NULL && i < argumentSize)
	{
		buf[i++] = nextArgument;
		nextArgument = strtok(NULL, " ");

	}
	//Program should never enter this with the current max limits
	if (i >= argumentSize)
	{
		printf("Unexpected error.\n");
		return 0;
	}

	buf[i] = NULL;

	*arguments = buf;
	return i;
}

//Checks if the given array of arguments make a compound command
int hasPipe(char** arguments)
{
	int i = 0;
	while (arguments[i] != NULL)
	{
		if (arguments[i][0] == '|')
			return 1;
		i++;
	}

	return 0;
}

//For a compound commmands argument array,
//finds and returns the first command (the one before the '|' character)
char** firstCommandOfPipe(char** arguments, int numArguments)
{
	char** firstCommandArguments = malloc(sizeof(char *) * (numArguments + 1));
	char* argument = NULL;
	int i = 0;
	while (arguments[i] != NULL)
	{
		if (arguments[i][0] == '|')
		{
			firstCommandArguments[i] = NULL;
			break;
		}
		argument = malloc(sizeof(char) * MAX_COMMAND_SIZE); 
		strcpy(argument, arguments[i]);
		firstCommandArguments[i] = argument;
		i++;
	}
	firstCommandArguments[i] = NULL;

	return firstCommandArguments;
}

//For a compound commmands argument array,
//finds and returns the second command (the one after the '|' character)
char** secondCommandOfPipe(char** arguments, int numArguments)
{
	char** secondCommandArguments = malloc(sizeof(char *) * numArguments);
	char* argument = NULL;
	int i = 0;

	while (arguments[i][0] != '|')
	{
		i++;
	}
	i++;

	int pos = 0;
	while (arguments[i] != NULL)
	{
		if (arguments[i][0] == '|') //Shell doesn't support more than two pipes
		{
			secondCommandArguments[pos] = NULL;
			break;
		}
		argument = malloc(sizeof(char) * MAX_COMMAND_SIZE); 
		strcpy(argument, arguments[i]);
		secondCommandArguments[pos] = argument;
		i++; pos++;
	}
	secondCommandArguments[pos] = NULL;

	return secondCommandArguments;
}

void runNormalMode()
{
	char* buf = NULL;
	char** arguments = NULL;
	char** firstCommandArguments = NULL;
	char** secondCommandArguments = NULL;

	struct timeval start_time;
	struct timeval end_time;

	int numArguments = 0;

	int pipe1[2];

	while (1)
	{
		printf("isp$ ");
		buf = ispReadLine(buf);
		numArguments = ispParseLine(buf, &arguments);
		gettimeofday(&start_time, NULL);

		if (!hasPipe(arguments)) //If the command is not compound
		{
			pid_t pid1 = 0;
			if ((pid1 = fork()) == 0)
				execvp(arguments[0], arguments);

			int status;
			waitpid(pid1, &status, 0);
		}
		else //If the command is compound
		{
			//I had to create another pipe for each execution,
			//otherwise I wasn't able to reopen the closed file descriptors.
			pipe(pipe1);

			firstCommandArguments = firstCommandOfPipe(arguments, numArguments);
			secondCommandArguments = secondCommandOfPipe(arguments, numArguments);

			pid_t pid1 = 0;
			pid_t pid2 = 0;

			if ((pid1 = fork()) == 0)
			{
				dup2(pipe1[1],1);
				//Close unused end
				close(pipe1[0]);
				execvp(firstCommandArguments[0], firstCommandArguments);
			}
			if ((pid2 = fork()) == 0)
			{
				dup2(pipe1[0], 0);
				//Close unused end
				close(pipe1[1]);
				execvp(secondCommandArguments[0], secondCommandArguments);
			}

			int status;
			//Close both ends since main is not involved.
        	close(pipe1[0]);
        	close(pipe1[1]);

        	//Waiting for the second execution is enough as it will end after the first.
        	waitpid(pid2, &status, 0);

			//Free firstCommandArguments and secondCommandArguments
			int i = 0;
			while (firstCommandArguments[i] != NULL)
			{
				free(firstCommandArguments[i]);
				i++;
			}
			i = 0;
			while (secondCommandArguments[i] != NULL)
			{
				free(secondCommandArguments[i]);
				i++;
			}
			free(firstCommandArguments);
			free(secondCommandArguments);
		}

		gettimeofday(&end_time, NULL);
		double elapsedTime = (end_time.tv_sec - start_time.tv_sec) +
  		(end_time.tv_usec - start_time.tv_usec) / 1000000.0;

		//printf("\nelapsed time: %f\n", elapsedTime);


		//Free dynamically allocated memory

		/* Not needed since apparently strtok does not allocate memory
		for (i = 0; i < numArguments; i++)
			free(arguments[i]);
		*/
		free(arguments);
		free(buf);
	}
}

void runTappedMode(int N)
{
	char* buf = NULL;
	char** arguments = NULL;
	char** firstCommandArguments = NULL;
	char** secondCommandArguments = NULL;

	struct timeval start_time;
	struct timeval end_time;

	int numArguments = 0;

	int pipe1[2]; //Pipe between child1 and main
	int pipe2[2]; //Pipe between child2 and main
	char readPipeBuf[4096];

	while (1)
	{
		printf("isp$ ");
		buf = ispReadLine(buf);
		numArguments = ispParseLine(buf, &arguments);
		gettimeofday(&start_time, NULL);
		int charCount = 0, readCallCount = 0, writeCallCount = 0;

		if (!hasPipe(arguments))
		{
			printf("In tapped mode, only compound commands are accepted.\n");
		}
		else
		{
			//I had to create new pipes for each execution,
			//otherwise I wasn't able to reopen the closed file descriptors.
			pipe(pipe1);
			pipe(pipe2);

			firstCommandArguments = firstCommandOfPipe(arguments, numArguments);
			secondCommandArguments = secondCommandOfPipe(arguments, numArguments);

			pid_t pid1 = 0;
			pid_t pid2 = 0;

			if ((pid1 = fork()) == 0)
			{
				dup2(pipe1[1],1);

				//Close unused pipe ends
				close(pipe1[0]);
				close(pipe2[0]);
				close(pipe2[1]);

				execvp(firstCommandArguments[0], firstCommandArguments);
			}
			if ((pid2 = fork()) == 0)
			{
				dup2(pipe2[0], 0);

				//Close unused pipe ends
				close(pipe1[0]);
				close(pipe1[1]);
				close(pipe2[1]);

				execvp(secondCommandArguments[0], secondCommandArguments);
			}

			//Close unused pipe ends
        	close(pipe1[1]);
        	close(pipe2[0]);

			int bytesRead = read(pipe1[0], &readPipeBuf,N);
			readCallCount++;
			charCount += bytesRead;
			while (bytesRead > 0) //Keep reading from the pipe until there is nothing to read
			{
				//printf("here %d %c\n", bytesRead, readPipeBuf[0]);
				write(pipe2[1], readPipeBuf, bytesRead);
				bytesRead = read(pipe1[0], &readPipeBuf, N);

				readCallCount++;
				writeCallCount++;
				charCount += bytesRead;
			}

			//Close the pipe ends to indicate the end of input.
        	close(pipe1[0]);
        	close(pipe2[1]);

			int status;
        	waitpid(pid1, &status, 0);
        	waitpid(pid2, &status, 0);


			//Free firstCommandArguments and secondCommandArguments
			int i = 0;
			while (firstCommandArguments[i] != NULL)
			{
				free(firstCommandArguments[i]);
				i++;
			}
			i = 0;
			while (secondCommandArguments[i] != NULL)
			{
				free(secondCommandArguments[i]);
				i++;
			}
			free(firstCommandArguments);
			free(secondCommandArguments);


			//Print statistics
			gettimeofday(&end_time, NULL);
			double elapsedTime = (end_time.tv_sec - start_time.tv_sec) +
	  		(end_time.tv_usec - start_time.tv_usec) / 1000000.0;

			printf("\ncharacter-count: %d\n", charCount);
			printf("read-call-count:: %d\n", readCallCount);
			printf("write-call-count: %d\n", writeCallCount);
			//printf("elapsed time: %f\n", elapsedTime);
		}

		//Free dynamically allocated memory
		/* Not needed since apparently strtok does not allocate memory
		for (i = 0; i < numArguments; i++)
			free(arguments[i]);
		*/
		free(arguments);
		free(buf);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf ("Must have 3 arguments. Correct usage is \n\tisp <N> <mode>\n");
		return 0;
	}

	int N = atoi(argv[1]);
	int mode = atoi(argv[2]);

	if (mode == 1)
	{
		runNormalMode();

	}
	else if (mode == 2)
	{
		if (N >= 1 && N <= 4096)
		{
			runTappedMode(N);
		}
		else
		{
			printf ("N must be between 1 and 4096. Correct usage is \n\tisp <N> <mode>\n");
		}
	}
	else
	{
		printf ("Mode must be 1 or 2. Correct usage is \n\tisp <N> <mode>\n");
	}

}
