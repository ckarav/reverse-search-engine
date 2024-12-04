

#ifndef PRIMITVE_DATA_STRUCT
#define PRIMITVE_DATA_STRUCT

typedef struct node_int_stack//node for int list//
{
	int data;
	int data2;
	struct node_int_stack* next;
}node_int_stack;

typedef struct list_int_stack//int list//
{
	node_int_stack* head;
	node_int_stack* tail;
	node_int_stack* itterator;
	int length;
}list_int_stack;

/**list of int implementation**/
void print_list_int(list_int_stack* list);//prints the list according to allocated nodes//
void print_list_int2(list_int_stack* list);//prints the list according to the length of the list//
void init_list_int(list_int_stack* list);//initializes vars to null values//
void add_list_int(list_int_stack* list,int data);//adds the data to a new node at the tail of the list//
void add_list_int2(list_int_stack* list,int data,int data2);//adds the datas to a new node at the tail of the list//
void delete_list_int(list_int_stack* list);//deallocs the whole list//
void flush_list_int(list_int_stack*list);//sets the list length to zero and the itterator to point at the head//
list_int_stack* copy_list_int(list_int_stack*original);//copies the original list to the already  list copy//copy arg should be a NULL pointer//

/**list itterator**/
void initit_list_int(list_int_stack*list);//points the list itterator to the head of the list//
void pushit_list_int(list_int_stack* list);//pushes the list itterator to the next node//
int getit_list_int(list_int_stack* list);//gets the itterator data//
int getit_list_int2(list_int_stack* list);//gets the itterator data2//
node_int_stack* get_ints_pointer(list_int_stack *list );//gets the node that the itterator points to//

#endif