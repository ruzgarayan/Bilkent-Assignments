

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "sbmem.h"

int main(int argc, char *argv[])
{
    int memSize, ret;

    if (argc == 2)
    	memSize = atoi(argv[1]);
    else
    	memSize = 32768;

    ret = sbmem_init(memSize); 

    if (ret == 0)
    	printf ("memory segment is created and initialized \n");
    else
    	printf ("memory segment could not be created! \n");

    return (0); 
}
