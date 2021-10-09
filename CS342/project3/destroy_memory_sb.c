

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "sbmem.h"

int main()
{
	int ret; 

    ret = sbmem_remove(); 

    if (ret == 0)
    {
    	printf ("memory segment is removed from the system. System is clean now.\n");
    	printf ("you can no longer run processes to allocate memory using the library\n"); 
    }
    else
    {
    	printf ("memory segment could not be removed from the system.\n");
    }

    return (0); 
}
