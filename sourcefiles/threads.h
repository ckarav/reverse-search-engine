#ifndef TRHEADS_H
#define THERADS_H
#include "core.h"


enum JobType
{	
	MATCH_DOC,
	GET_RESULT,
	BARRIER,
	EXIT,
};


typedef struct Job
{
	enum JobType type;
	void* JobParams; 
}Job;

typedef struct MatchDocParams
{
	DocID doc_id;
	char* str;
	int GlobArrPos; //position in global arrays to write the result//
}MatchDocParams;

typedef struct GetNextParams
{
	DocID* doc_id;
	unsigned int*  p_num_res;
	QueryID**      p_query_ids;
	
}GetNextParams;


typedef struct tail_node
{
	Job* data;
	struct tail_node* next;
	struct tail_node* previews;
}tail_node;

typedef struct JobsTail
{
	tail_node* head;
	tail_node* tail;
	int length;
}JobsTail;

/****TAIL IMPLEMENTATION FOR FIFO********/
void init_tail(JobsTail* tail);//initializes tha tail//
void push_tail(JobsTail* tail,Job* job);//pushes a Job in the tail//
Job* pop_tail(JobsTail* tail);//pops a job from the tail//
void print_tail(JobsTail* tail);//print the tail//

tail_node* create_tailnode(Job* job); //creates a tail_node that contains the Job as data, and returns the new address(node is allocated inside routine)//
void push_tailnode(JobsTail* tail,tail_node* node);//pushes a tailnode into the tail//
tail_node* pop_tailnode(JobsTail* tail);//pops a tailnode from the tail//
Job* destroy_tailnode(tail_node* node);//destroys a tailnode and returns the address of the data//
#endif
