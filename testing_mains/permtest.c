#include "permtest.h"
#include <stdio.h>

int between(int Lrange,int Rrange,int arg)
{
	if (arg>=Lrange&&arg<=Rrange) 
	{
		return 1;
	}
	return 0;
}

void permtest(int (*function)(int),int ALrange,int ARrange,int RLrange,int RRrange) //overwrite prototype
{
	int i;
	for (i=ALrange;i<=ARrange;i++) 
	{
		if(!between(RLrange,RRrange,(*function)(i)))
		{	
			printf("Function with address %p returned unexpected value %d for arguments:[%d]\n",function,(*function)(i),i);
		}
	}
	return;
}

int test(int i)
{
	return i;
}

int main()
{
	permtest(test,-10,10,-8,11);
}