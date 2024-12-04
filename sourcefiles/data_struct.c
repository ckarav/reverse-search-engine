/*
 *	The followed data structures were implemented in the context of the Word structure.
 *  Functions were designed and implemented according to the needs of the project (theoretically max efficiency).
 *
 */

#include "data_struct.h"
#include "defn.h"
#include "defn_impl.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********LIST OF STRUCTURE WORD IMPLEMENTATION*************************/
/*The list implementation was done with specific usage in mind************/
/*It allows to add nodes at the end of the list in O(1) time**************/
/*It allows to find if a word context exists in the list in O(n)**********/
/*It allows to get a word by index in O(n) time***************************/
/*It allows to init an itterator to the head of the list******************/
/*It allows to push an itterator to the next node of the list*************/
/*It allos to get the node->data of the node that the itterator points to*/
/*Deleting a node form the list is not supported**************************/
/*Printing the list content is allowed for debugging purposes*************/
/*************************************************************************/


void init_list_Words(list_Words* list)
{
	list->length = 0;
	list->head = NULL;
	list->tail = NULL;
	list->itterator=NULL;
}

void initit_list_Words(list_Words* list)
{
	list->itterator = list->head;
}

void pushit_list_Words(list_Words* list)
{
	if(list->itterator)
		list->itterator=list->itterator->next;
}

Word* getit_list_Words(list_Words* list)
{
	if(list->itterator)
	{
		return list->itterator->data;
	}
	return NULL;
}

void add_list_Words(list_Words* list,Word* word)
{
	node_Words* node;
	
	node = malloc(sizeof(node_Words));
	node->data = word;
	node->next = NULL;
	if(list->length==0)
	{
		list->head = node;
		list->tail = node;
	}
	else 
	{
		list->tail->next = node;
		list->tail = node;
	}
	list->length++;
}

int find_list_Words(list_Words* list,Word* word)
{
	int i = 0;
	node_Words* pointer;
	
	pointer = list->head;
	for(i=0;i<list->length;i++)
	{
		if(pointer->data->length==word->length) //opt to avoid checking strs with different length//
		{
			if(strcmp(pointer->data->characters,word->characters)==0)
			{
				return 1;
			}
		}
		pointer = pointer->next;
	}
	return 0;
}

Word* get_list_Words(list_Words* list,int index) //depreciated, use getit_list_Words insted//
{
	int i=0;
	node_Words* pointer;
	
	pointer = list->head;
	for(i=0;i<list->length;i++)
	{
		if(i==index)
		{
			return pointer->data; 
		}
		pointer=pointer->next;
	}
	return NULL;
}

void delete_list_Words(list_Words* list)
{
	int i = 0;
	node_Words* pointer;
	node_Words* buffer;
	pointer=list->head;
	for(i=0;i<list->length;i++)
	{
		buffer = pointer;
		pointer = pointer->next;
		free(buffer);
		
	}
	buffer =NULL;
}

void destroy_list_Words(list_Words* list)
{
	int i = 0;
	node_Words* pointer;
	node_Words* buffer;
	pointer=list->head;
	for(i=0;i<list->length;i++)
	{
		buffer = pointer;
		pointer = pointer->next;
		DestroyWord(buffer->data);
		free(buffer->data);
		free(buffer);
		
	}
}

void print_list_Words(struct list_Words* list)
{
	int i=0;
	struct node_Words* pointer = list->head;
	
	printf("List has length %d\n",list->length);
	for(i=0;i<list->length;i++)
	{
		printf("(%s,%d)->",pointer->data->characters,pointer->data->length);
		pointer = pointer->next;
	}
	printf("NULL\n");
}


/********************************************************/
/*********BINARY TREES IMPLEMENTATION********************/
/*It allows to initialize a bintree struct fields********/
/*It allows to add a bintree node to the tree************/
/*It allows to find a Word in the Tree recursively(DFS)**/
/*It allows to add a Word in the Tree only if its unique*/
/*It allows to destroy the tree struct recursively(DFS)**/
/*It allows to print the tree context recursively (DFS)**/
/********************************************************/

/********WORD POINTER TREE***/ //[@temp] change the compiler to g++ , use inheritance and use the same functions for both words and entries//

void init_bintree(bintree_Words* tree)
{
	tree->root=NULL;
}

int add_bintree(bintree_Words* tree,Word* word)
{
	nodbintree_Words* pointer;
	
	pointer = tree->root;
	if(!tree->root)
	{
		tree->root = malloc(sizeof(nodbintree_Words));
		tree->root->data= word;
		tree->root->right = NULL;
		tree->root->left = NULL;
		tree->num_of_words++;
		return 1;
	}
	else
	{
		while(1)
		{
			if(strcmp(pointer->data->characters,word->characters)<0)
			{
				if(pointer->right)
				{
					pointer = pointer->right;
				}
				else
				{
					pointer->right = malloc(sizeof(nodbintree_Words));
					pointer->right->data = word;
					pointer->right->right=NULL;
					pointer->right->left=NULL;
					tree->num_of_words++;
					return 1;
				}
			}
			else 
			{
				if(pointer->left)
				{
					pointer = pointer->left;
				}
				else 
				{
					pointer->left=malloc(sizeof(nodbintree_Words));
					pointer->left->data =word;
					pointer->left->right=NULL;
					pointer->left->left=NULL;
					tree->num_of_words++;
					return 1;
				}
			}
		}
	}
	return 0;
}


int find_bintree(bintree_Words* tree,Word* word)
{
	int steps=0;
	int strcmpret=0;
	nodbintree_Words* pointer;
	
	pointer = tree->root;
	while (1)
	{
		if(pointer)
		{
			if((strcmpret=strcmp(pointer->data->characters,word->characters))==0)
			{
				printf("found word %s in tree with %d steps\n",word->characters,steps);
				return 1;
			}
			else if(strcmpret<0)
			{
				pointer=pointer->right;
				steps++;
			}
			else 
			{
				pointer=pointer->left;
				steps++;
			}
		}
		else 
		{
			printf("NOT found word%s in tree after %d steps\n",word->characters,steps);
			return 0;
		}
	}	
}

int addnew_bintree(bintree_Words* tree,Word* word)
{
	nodbintree_Words* pointer;
	pointer = tree->root;
	int strcmpret=0;
	if(!tree->root)
	{
		tree->root = malloc(sizeof(nodbintree_Words));
		tree->root->data = word;
		tree->root->right=NULL;
		tree->root->left =NULL;
		tree->num_of_words++;
		return 1;
	}
	else
	{
		while (1)
		{
			if(!(strcmpret=strcmp(pointer->data->characters,word->characters)))  //case we found the word dup allong the path
			{
				word->length=0;//marks that the word is a duplicate so it could be destroyed later at a global scope//
				return 0;
			}
			else if(strcmpret<0)
			{
				if(pointer->right)
				{
					pointer=pointer->right;
				}
				else
				{
					pointer->right =malloc(sizeof(nodbintree_Words));
					pointer->right->data=word;
					pointer->right->right=NULL;
					pointer->right->left=NULL;
					tree->num_of_words++;
					return 1;
				}
			}
			else
			{
				if(pointer->left)
				{
					pointer=pointer->left;
				}
				else 
				{
					pointer->left = malloc(sizeof(nodbintree_Words));
					pointer->left->data = word;
					pointer->left->right = NULL;
					pointer->left->left =NULL;
					tree->num_of_words++;
					return 1;
				}
			}
		}
	}
	return 0;
}

void delete_bintree_rec(nodbintree_Words* parent)
{
	if(parent->right)
	{
		delete_bintree_rec(parent->right);
		free(parent->right);
		parent->right =NULL;
	}
	if(parent->left)
	{
		delete_bintree_rec(parent->left);
		free(parent->left);
		parent->left =NULL;
	}
}

void delete_bintree(bintree_Words* tree) //[opt]:avoid recursive method,use stack of (node* paren_addr)//
{
	if(tree->root)
	{
		delete_bintree_rec(tree->root);
		free(tree->root);
		tree->root = NULL;
	}
}

void print_bintree_rec(nodbintree_Words* parent)//[opt]:avoid rec method,use stack of(node* parent,int vis_rght,int vis_left)//
{
	printf("(%s,%d)\n",parent->data->characters,parent->data->length);
	if(parent->right)
	{
		print_bintree_rec(parent->right);
	}
	if(parent->left)
	{
		print_bintree_rec(parent->left);
	}
	return;
}

void print_bintree(bintree_Words* tree)
{
	printf("Binary Tree Contains %d Words\n",tree->num_of_words);
	if(tree->root)
	{
		print_bintree_rec(tree->root);
	}
}
/********ENTRY POINTER TREE***/ //[@temp] change the compiler to g++ , use inheritance and use the same functions for both words and entries//
void init_bintreeEntry(bintree_Entries* tree)
{
	tree->root=NULL;
}

int add_bintreeEntry(bintree_Entries* tree,Entry* entry)
{
	nodbintree_Entries* pointer;
	
	pointer = tree->root;
	if(!tree->root)
	{
		tree->root = malloc(sizeof(nodbintree_Words));
		tree->root->data=entry;
		tree->root->right = NULL;
		tree->root->left = NULL;
		tree->num_of_words++;
		return 1;
	}
	else
	{
		while(1)
		{
			if(strcmp(pointer->data->word->characters,entry->word->characters)<0)
			{
				if(pointer->right)
				{
					pointer = pointer->right;
				}
				else
				{
					pointer->right = malloc(sizeof(nodbintree_Words));
					pointer->right->data = entry;
					pointer->right->right=NULL;
					pointer->right->left=NULL;
					tree->num_of_words++;
					return 1;
				}
			}
			else 
			{
				if(pointer->left)
				{
					pointer = pointer->left;
				}
				else 
				{
					pointer->left=malloc(sizeof(nodbintree_Words));
					pointer->left->data =entry;
					pointer->left->right=NULL;
					pointer->left->left=NULL;
					tree->num_of_words++;
					return 1;
				}
			}
		}
	}
	return 0;
}


Entry* find_bintreeEntry(bintree_Entries* tree,Word* word)
{
	int steps=0;
	int strcmpret=0;
	nodbintree_Entries* pointer;
	
	pointer = tree->root;
	while (1)
	{
		if(pointer)
		{
			if((strcmpret=strcmp(pointer->data->word->characters,word->characters))==0)
			{
				//printf("found word %s in tree with %d steps\n",entry->word->characters,steps);
				return pointer->data;
			}
			else if(strcmpret<0)
			{
				pointer=pointer->right;
				steps++;
			}
			else 
			{
				pointer=pointer->left;
				steps++;
			}
		}
		else 
		{
			//printf("NOT found word%s in tree after %d steps\n",entry->word->characters,steps);
			return NULL;
		}
	}	
}

int addnew_bintreeEntry(bintree_Entries* tree,Entry* entry)
{
	nodbintree_Entries* pointer;
	pointer = tree->root;
	int strcmpret=0;
	if(!tree->root)
	{
		tree->root = malloc(sizeof(nodbintree_Words));
		tree->root->data = entry;
		tree->root->right=NULL;
		tree->root->left =NULL;
		tree->num_of_words++;
		return 1;
	}
	else
	{
		while (1)
		{
			if(!(strcmpret=strcmp(pointer->data->word->characters,entry->word->characters)))  //case we found the word dup allong the path
			{
				//concat the payload lists//
				int i;
				//itterate over the qid_list of the entry
				initit_list_int(entry->payload->queryPos_list);
				for(i=0;i<entry->payload->queryPos_list->length;i++)
				{
					add_list_int2(pointer->data->payload->queryPos_list,getit_list_int(entry->payload->queryPos_list),getit_list_int2(entry->payload->queryPos_list));//add to the list
					pushit_list_int(entry->payload->queryPos_list);//push the itterator
					
				}
				entry->word->length = 0;//mark the entry to be destroyed by the meta//
				return 0;
			}
			else if(strcmpret<0)
			{
				if(pointer->right)
				{
					pointer=pointer->right;
				}
				else
				{
					pointer->right =malloc(sizeof(nodbintree_Words));
					pointer->right->data=entry;
					pointer->right->right=NULL;
					pointer->right->left=NULL;
					tree->num_of_words++;
					return 1;
				}
			}
			else
			{
				if(pointer->left)
				{
					pointer=pointer->left;
				}
				else 
				{
					pointer->left = malloc(sizeof(nodbintree_Words));
					pointer->left->data = entry;
					pointer->left->right = NULL;
					pointer->left->left =NULL;
					tree->num_of_words++;
					return 1;
				}
			}
		}
	}
	return 0;
}

void delete_bintree_recEntry(nodbintree_Entries* parent)
{
	if(parent->right)
	{
		delete_bintree_recEntry(parent->right);
		free(parent->right);
		parent->right =NULL;
	}
	if(parent->left)
	{
		delete_bintree_recEntry(parent->left);
		free(parent->left);
		parent->left =NULL;
	}
}

void delete_bintreeEntry(bintree_Entries* tree) //[opt]:avoid recursive method,use stack of (node* paren_addr)//
{
	if(tree->root)
	{
		delete_bintree_recEntry(tree->root);
		free(tree->root);
		tree->root = NULL;
	}
}

void print_bintree_recEntry(nodbintree_Entries* parent)//[opt]:avoid rec method,use stack of(node* parent,int vis_rght,int vis_left)//
{
	printf("(%s,%d)\n",parent->data->word->characters,parent->data->word->length);
	if(parent->right)
	{
		print_bintree_recEntry(parent->right);
	}
	if(parent->left)
	{
		print_bintree_recEntry(parent->left);
	}
	return;
}

void print_bintreeEntry(bintree_Entries* tree)
{
	printf("Binary Tree Contains %d Words\n",tree->num_of_words);
	if(tree->root)
	{
		print_bintree_recEntry(tree->root);
	}
}


/*****HASHMAP with SEPARATE CHAINING-BINARY TREES IMPLEMENTATION********/

void init_HashMap_SC_BT(HashMap_SC_BT* hashmap,int size)
{
	int i=0;
	hashmap->length=size;
	hashmap->num_of_words=0;
	hashmap->map = malloc(sizeof(bintree_Words)*size);
	for(i=0;i<size;i++)
	{
		init_bintree(&hashmap->map[i]);
	}
}

void add_HashMap_SC_BT(HashMap_SC_BT* hashmap,Word* word)
{
	hashmap->num_of_words+=(addnew_bintree(&(hashmap->map[word->hash%(unsigned)hashmap->length]),word));
}

void print_HashMap_SC_BT(HashMap_SC_BT* hashmap)
{
	int i=0;
	for(i=0;i<hashmap->length;i++)
	{
		print_bintree(&(hashmap->map[i]));
	}
}

void delete_HashMap_SC_BT(HashMap_SC_BT* hashmap)
{
	int i=0;
	for(i=0;i<hashmap->length;i++)
	{
		delete_bintree(&(hashmap->map[i]));
	}
	free(hashmap->map);
	hashmap->map=NULL;
}

/******HASHMAP with SEPARATE CHAINING-BINARY TREES IMPLEMENTATION*********/

void init_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap,int size)
{
	int i=0;
	hashmap->length=size;
	hashmap->num_of_words=0;
	hashmap->map = malloc(sizeof(bintree_Entries)*size);
	for(i=0;i<size;i++)
	{
		init_bintreeEntry(&hashmap->map[i]);
	}
}

void add_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap,Entry* entry)
{
	hashmap->num_of_words+=(addnew_bintreeEntry(&(hashmap->map[entry->word->hash%(unsigned)hashmap->length]),entry));
}

void print_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap)
{
	int i=0;
	for(i=0;i<hashmap->length;i++)
	{
		print_bintreeEntry(&(hashmap->map[i]));
	}
}

void delete_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap)
{
	int i=0;
	for(i=0;i<hashmap->length;i++)
	{
		delete_bintreeEntry(&(hashmap->map[i]));
	}
	free(hashmap->map);
	hashmap->map=NULL;
}

Entry* find_Hashmap_SC_BT_Entry(HashMap_SC_BTEntry* hashmap,Word* word)
{
	return find_bintreeEntry(&(hashmap->map[word->hash%hashmap->length]),word);
}
