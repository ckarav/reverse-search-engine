#include "data_sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
int main()
{
	int i;
	int array[10];
	srand(time(NULL));
	
	for(i=0;i<10;i++)
	{
		//printf("uo");
		array[i] = rand()%10;
		printf("array[ %d ] is %d\n",i,array[i]);
	}
	
	printf("Radix is %d\n",RadixSort(array,10));
	
	
}

