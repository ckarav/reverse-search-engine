
#include "data_sort.h"
#include "data_struct.h"
#include "primitive_data_struct.h"
#include <stdlib.h>
#include <stdio.h>
/*****************RADIX SORT (LOW ROUNDED) MEDIAN******************************/
/*Radix Sort for lexigraphical sorting of intergers array**********************/
/*The function returns the Median(low rounding) of the array given as argument*/
/*the sorted array does NOT overwrite the argument array***********************/
/*This is a the first draft.The implementation has been tested as complete ****/
/*but a optimizations have not been performed yet******************************/
/******************************************************************************/
int RadixSort(int* array,int array_len)
{
	int i,j,k;
	int counter=0;
	int* array_alias=malloc(sizeof(int)*array_len); //alloc the allias array//
	list_int_stack* stack_array = malloc(sizeof(list_int_stack)*10); //alloc the stack array//
	int itt_limit = GetDigitsNum(GetMax(array,array_len));
	
	for(i=0;i<array_len;i++) //initialize the data structures//
	{
		array_alias[i] = array[i];//initialize the allias array//
	}
	for(i=0;i<10;i++)
	{
		init_list_int(&stack_array[i]); //initialize the stacks//
	}
	for(k=0;k<itt_limit;k++) //itterations number is the num of digits of the max num//
	{
		for(i=0;i<array_len;i++)//hash the nums to the buckets//
		{
			add_list_int(&stack_array[GetDigit(array_alias[i],k)],array_alias[i]);
		}
		counter=0;
		for(i=0;i<10;i++) //gather the hashed elements and copy them to the alias array with their new order//
		{
			initit_list_int(&stack_array[i]);
			for(j=0;j<stack_array[i].length;j++)
			{
				array_alias[counter] = getit_list_int(&stack_array[i]);
				pushit_list_int(&stack_array[i]);
				counter++;
			}
		}
		for (i=0;i<10;i++) //flush the stacks to overwrite the new data on the next itteration
		{
			flush_list_int(&stack_array[i]);
		}
	}
	for(i=0;i<10;i++) //dealloc the stacks of the array//
	{
		delete_list_int(&stack_array[i]);
	}
	free(stack_array);//dealloc the stack array//
	i = array_alias[array_len/2];//store the median (down rounding) to a temp var//
	free(array_alias);//dealloc the array alias//
	return i;//return the result//
	 
}

list_int_stack* RadixSortList(list_int_stack* list)
{
	int i,j,k;
	int counter=0;
	int* array_alias=malloc(sizeof(int)*list->length); //alloc the allias array//
	list_int_stack* stack_array = malloc(sizeof(list_int_stack)*10); //alloc the stack array//
	initit_list_int(list);
	for(i=0;i<list->length;i++) //initialize the data structures//
	{
		array_alias[i] = getit_list_int(list);//initialize the allias array//
		pushit_list_int(list);//push the itterator//
	}
	int itt_limit = GetDigitsNum(GetMax(array_alias,list->length));
	for(i=0;i<10;i++)
	{
		init_list_int(&stack_array[i]); //initialize the stacks//
	}
	for(k=0;k<itt_limit;k++) //itterations number is the num of digits of the max num//
	{
		for(i=0;i<list->length;i++)//hash the nums to the buckets//
		{
			add_list_int(&stack_array[GetDigit(array_alias[i],k)],array_alias[i]);
		}
		counter=0;
		for(i=0;i<10;i++) //gather the hashed elements and copy them to the alias array with their new order//
		{
			initit_list_int(&stack_array[i]);
			for(j=0;j<stack_array[i].length;j++)
			{
				array_alias[counter] = getit_list_int(&stack_array[i]);
				pushit_list_int(&stack_array[i]);
				counter++;
			}
		}
		for (i=0;i<10;i++) //flush the stacks to overwrite the new data on the next itteration
		{
			flush_list_int(&stack_array[i]);
		}
	}
	for(i=0;i<10;i++) //dealloc the stacks of the array//
	{
		delete_list_int(&stack_array[i]);
	}
	
	initit_list_int(list);
	for(i=0;i<list->length;i++) //initialize the data structures//
	{
		get_ints_pointer(list)->data=array_alias[i];//initialize the allias array//
		pushit_list_int(list);//push the itterator//
	}
	
	free(stack_array);//dealloc the stack array//
	free(array_alias);//dealloc the array alias//
	return list;
	
}



//generic purpose implementations to avoid the usage of math.h//
unsigned int GetDigit(int num,int index) 
{
	int i;
	for (i=0;i<index;i++) 
	{
		num/=10;
	}
	return (unsigned)num%10;
	
}

int GetDigitsNum(int num) //also known as log10//
{
	 int counter=0;
	while(num>0)
	{
		num/=10;
		counter++;
	}
	return counter;
	
}

int GetMax(int* array,int length)//returns |max| element form an array of integers//
{
	int i;
	int max=0;
	for(i=0;i<length;i++)
	{
		if(array[i]<0)
		{
			if((-1)*array[i]>max)
			{
				max = (-1)*array[i];
			}
		}
		else
		{
			if(array[i]>max)
			{
				max = array[i];
			}
		}
			   
			   
	}
	return max;
}