#include "hash.h"
#include <stdio.h>
int M_HashFunction(Word key,int table_size)
{
	return M_HashCode(key)%table_size;	
}

unsigned int M_HashCode(Word key) 
{
	unsigned int INITIAL_VAL = 5381;
	unsigned int MULTIPLIER = 26; //coresponds to the num of different ascii characters the str might have//
	int i=0;
	int j=0;
	unsigned int power=1;
	unsigned int hash;
	hash = INITIAL_VAL*MULTIPLIER;
	
	for(i=0;i<key.length;i++)
	{
		power=1;
		for(j=0;j<key.length-(i+1);j++)
		{
			power*=MULTIPLIER;
		}
		hash+=(unsigned)key.characters[i]*power;
	}
	return hash;
}