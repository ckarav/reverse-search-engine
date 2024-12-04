/*
 *	The followed data structures were implemented for a specific context,the Word structure.
 *  Functions and datatypes were designed and implemented according to the needs of the project, for (theoretical) maximum efficiency.
 *
 */

#include "defn.h"
#include "index.h"

#ifndef DATA_STRUCT 
#define DATA_STRUCT

typedef struct node_Words //node for word list//
{  
	Word* data;
	struct node_Words* next;
}node_Words;

typedef struct list_Words //word list//
{ 
	node_Words* head;
	node_Words* tail;
	node_Words* itterator;
	int length;
}list_Words;

typedef struct nodbintree_Words //node for word binary tree//
{ 
	Word* data ;
 	struct nodbintree_Words* left;
	struct nodbintree_Words* right;
}nodbintree_Words;

typedef struct nodbintree_Entries //node for word binary tree//
{ 
	Entry* data ;
 	struct nodbintree_Entries* left;
	struct nodbintree_Entries* right;
}nodbintree_Entries;


typedef struct bintree_Words //word binary tree 
{ 
	nodbintree_Words* root;
	int num_of_words;
}bintree_Words;

typedef struct bintree_Entries //word binary tree 
{ 
	nodbintree_Entries* root;
	int num_of_words;
}bintree_Entries;

typedef struct HashMap_SC_BT //hashmap with separate chaining and binary trees//
{
	bintree_Words* map;
	int length;
	int num_of_words;
}HashMap_SC_BT;

typedef struct HashMap_SC_BTEntry //hashmap with separate chaining and binary trees//
{
	bintree_Entries* map;
	int length;
	int num_of_words;
}HashMap_SC_BTEntry;


/**list of word pointer implementation*/
void init_list_Words(struct list_Words* list); //initialization of list,all pointers get value//
void add_list_Words(struct list_Words* list,Word* word);//adds a word to the list,word must be created//
Word* get_list_Words(list_Words* list,int index); //returns the node->data of the node at the position index//
void delete_list_Words(list_Words* list);//deallocates the list nodes//
void destroy_list_Words(list_Words* list);//deallocates the list nodes and node context//
int find_list_Words(list_Words* list,Word* word);//returns 1 if the word is in the list, else 0//
void print_list_Words(struct list_Words* list); //prints the pointer values of the list nodes//

/** list itterator */
void initit_list_Words(list_Words* list); //points itterator of list to the head of the list//
void pushit_list_Words(list_Words* list);//pushes itterator to the next node of the list//
Word* getit_list_Words(list_Words* list);//returns the node-data of the node that the itterator points to//

/*binary tree with word pointer as context implementation*/
void init_bintree(bintree_Words* tree); //initialized tree (root pointer gets null value and wordcounter also//
int add_bintree(bintree_Words* tree,Word* word);//adds a word to the bintree according to the return of strcmp (greater values go right)//
int addnew_bintree(bintree_Words* tree,Word* word);//same as add(look above) but if the same word context is found along the path, the function returns with 0,otherwise 1//
int find_bintree(bintree_Words* tree,Word* word);//[DebugFunction]:finds a word inside the bintree,returns 1 if found and 0 otherwise,a message is printed in both cases//
void print_bintree(bintree_Words* tree); //[DebugFunction]:prints the tree context Recursively Topdown by DFS,Right path is taken first//
void delete_bintree(bintree_Words* tree);//Deletes the bintree Recursively BottomUp by DFS,Right path is taken first,context is not deallocated, only the structure. 

/*binary tree with entry pointer as context implementation*///[@temp]should avoid if more data types//
void init_bintreeEntry(bintree_Entries* tree); //initialized tree (root pointer gets null value and wordcounter also//
int add_bintreeEntry(bintree_Entries* tree,Entry* word);//adds a word to the bintree according to the return of strcmp (greater values go right)//
int addnew_bintreeEntry(bintree_Entries* tree,Entry* word);//same as add(look above) but if the same word context is found along the path, the function returns with 0,otherwise 1//
Entry* find_bintreeEntry(bintree_Entries* tree,Word* word);//[DebugFunction]:finds a word inside the bintree,returns 1 if found and 0 otherwise,a message is printed in both cases//
void print_bintreeEntry(bintree_Entries* tree); //[DebugFunction]:prints the tree context Recursively Topdown by DFS,Right path is taken first//
void delete_bintreeEntry(bintree_Entries* tree);//Deletes the bintree Recursively BottomUp by DFS,Right path is taken first,context is not deallocated, only the structure. 

/*hashmap with separate chaining and binary trees as buckers implementation*/
void init_HashMap_SC_BT(HashMap_SC_BT* hashmap,int size); //initializes hashmap values to null values and allocates the hashtable by size param//
void add_HashMap_SC_BT(HashMap_SC_BT* hashmap,Word* word);//adds a word to the hashmap//
void print_HashMap_SC_BT(HashMap_SC_BT* hashmap);//[DebugFunction]:prints the hash map by printing all trees that it contains, starting from the base of the array//
void delete_HashMap_SC_BT(HashMap_SC_BT* hashmap);//Deletes the HashMap,by deleting the bintrees that it contains,starting from the base.Same with bintrees, the context is not destroyed, only the structure//

/*hashmeap with seperate cahining and binary trees as buckets implementation (Entries)*/
void init_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap,int size);
void add_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap,Entry* entry);//adds a word to the hashmap//
void print_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap);//[DebugFunction]:prints the hash map by printing all trees that it contains, starting from the base of the array//
void delete_HashMap_SC_BTEntry(HashMap_SC_BTEntry* hashmap);//Deletes the HashMap,by deleting the bintrees that it contains,starting from the base.Same with bintrees, the context is not destroyed, only the structure//
Entry* find_Hashmap_SC_BT_Entry(HashMap_SC_BTEntry* hashmap,Word* word);//return the list of qids that match with the word//
#endif
