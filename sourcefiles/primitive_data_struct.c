#include "primitive_data_struct.h"
#include <stdio.h>
#include <stdlib.h>

/**********LIST OF INTEGERS IMPLEMENTATION*************************************************/
/*The list implementation was done with specific usage in mind*****************************/
/*This implementation allows to:***********************************************************/
/*-print the list data starting from the beginning of the list*****************************/
/*-initialize the list*********************************************************************/
/*-add a new data to the end of the list***************************************************/
/*-destroy the list according to the space allocated by the nodes**************************/
/*-flush the list pointers to point at the head of the list and the length to be set to 0**/
/*so the list data can be overwriten by avoiding malloc/free operations********************/
/*-itterate starting on its head via an itterator******************************************/ 
/******************************************************************************************/

void print_list_int(list_int_stack* list)
{
	node_int_stack* pointer;
	pointer = list->head;
	while (pointer) 
	{
		printf("(%d)->",pointer->data);
		pointer=pointer->next;
	}
	printf("\n");
}

void print_list_int2(list_int_stack* list)
{
	node_int_stack* pointer;
	pointer = list->head;
	int i;
	for (i=0;i<list->length;i++)
	{
		printf("(%d,%d)->",pointer->data,pointer->data2);
		pointer = pointer->next;
	}
	printf("\n");
}

void init_list_int(list_int_stack* list)
{
	list->head=NULL;
	list->tail=NULL;
	list->itterator=NULL;
	list->length=0;
}

void add_list_int(list_int_stack* list,int data)
{
	if(list->length)
	{
		//if(!list->tail->next)
		{
			list->tail->next=malloc(sizeof(node_int_stack));
			list->tail->next->next=NULL;
		}
		list->tail->next->data=data;
		list->tail->next->data2=0;
		list->tail=list->tail->next;
	}
	else
	{
		//if(!list->tail)
		{
			list->tail=malloc(sizeof(node_int_stack));
			list->head = list->tail;
			list->tail->next=NULL;
		}
		list->tail->data=data;
		list->tail->data2=0;
	}
	list->length++;
}

void add_list_int2(list_int_stack* list,int data,int data2)
{
	if(list->length)
	{
		//if(!list->tail->next)
		{
			list->tail->next=malloc(sizeof(node_int_stack));
			list->tail->next->next=NULL;
		}
		
		list->tail->next->data=data;
		list->tail->next->data2=data2;
		list->tail=list->tail->next;
	}
	else
	{
		//if(!list->tail)
		{
			list->tail=malloc(sizeof(node_int_stack));
			list->head = list->tail;
			list->tail->next=NULL;
		}
		list->tail->data=data;
		list->tail->data2=data2;
	}
	list->length++;
	
}


void delete_list_int(list_int_stack* list)
{
	if(!list) return;
	node_int_stack* pointer;
	node_int_stack* mem_pointer;
	pointer=list->head;
	while (pointer)
	{
		mem_pointer=pointer->next;
		pointer->next = NULL;
		free(pointer);
		pointer=mem_pointer;
	}
	list->length=0;
	list->head=NULL;
	list->tail=NULL;
	list->itterator=NULL;
}


void flush_list_int(list_int_stack* list)
{
	list->length=0;
	list->tail = list->head;
	list->itterator = list->head;
}

void initit_list_int(list_int_stack* list)
{
	list->itterator = list->head;
}

void pushit_list_int(list_int_stack*list)
{
	if(list->itterator)
		list->itterator = list->itterator->next;
}

int getit_list_int(list_int_stack* list)
{
	if(list->itterator)
		return list->itterator->data;
	return -1;
}

int getit_list_int2(list_int_stack* list)
{
	if(list->itterator)
		return list->itterator->data2;
	return -1;
}


list_int_stack* copy_list_int(list_int_stack*original)
{
	list_int_stack* copy;
	node_int_stack* pointer;
	copy=malloc(sizeof(list_int_stack));
	
	pointer=original->head;
	init_list_int(copy);
	int i;
	for(i=0;i<original->length;i++)
	{
		add_list_int2(copy,pointer->data,pointer->data2);
		pointer=pointer->next;
	}
	return copy;
}

node_int_stack* get_ints_pointer(list_int_stack* list)
{
	return list->itterator;
}
