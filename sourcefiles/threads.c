
#include <string.h>
#include "threads.h"
#include <stdio.h>
#include "core.h"
#include <stdlib.h>
#include <pthread.h>
#include "defn.h"
#include "defn_impl.h"
#include "index.h"
#include "index_impl.h"
#include "data_struct.h"
#include "data_sort.h"
#include "cache.h"
#define BATCHSIZE 50

int hit_count=0;
int hit_miss=0;
void fillMap(EntryList *Entries_Threshn,int **bitMap);
void addResults(int** bitMap,list_int_stack *results,enum MatchType MT,int threshold);
void zerofy(int **bitMap, int rows);

enum ErrorCode thread_MatchDocument(DocID doc_id,const char*   doc_str,Cache* cache,int GlobArrPos);
enum ErrorCode thread_GetNextAvailRes(DocID* p_doc_id,unsigned int* p_num_res,QueryID** p_query_ids);

void* thread_ExecJob(Cache* cache)
{
	Job* ThreadJob;
	while(1)
	{
		sem_wait(semaphorFIFO);//wait until there is at least 1 job inside FIFO//
		tail_node* tailnode; 
		pthread_mutex_lock(&mutexFIFO);//lock the FIFO//
		tailnode = pop_tailnode(&FIFO);//pop the last node of FIFO//
		pthread_mutex_unlock(&mutexFIFO);//unlock the FIFO//
		ThreadJob = destroy_tailnode(tailnode); //get the Job from the node and delete the node//
		if(ThreadJob->type==MATCH_DOC)//Job to match a doc that is specified in the parameters//
		{
			thread_MatchDocument(((MatchDocParams*)ThreadJob->JobParams)->doc_id,((MatchDocParams*)ThreadJob->JobParams)->str,cache,((MatchDocParams*)ThreadJob->JobParams)->GlobArrPos);
			free(ThreadJob->JobParams);
			free(ThreadJob);
		}
		else if(ThreadJob->type==GET_RESULT)//Job to get the result for a doc and save it on the array specified in the parameters//
		{
			thread_GetNextAvailRes(((GetNextParams*)ThreadJob->JobParams)->doc_id,((GetNextParams*)ThreadJob->JobParams)->p_num_res,((GetNextParams*)ThreadJob->JobParams)->p_query_ids);
			free(ThreadJob->JobParams);
			free(ThreadJob);
		}
		else if(ThreadJob->type==BARRIER)//Job to wait on the barrier until all threads get there//
		{
			free(ThreadJob);
			pthread_barrier_wait(&Barrier);//wait until all threads get here//
			if(pthread_self()==MASTER)//the master thread should exit the thread_ExecJob to continue scheduling//
				break;
		}
		else if(ThreadJob->type==EXIT)//Job to exit the infinite loop//
		{
			free(ThreadJob);
			break;
		}
		
		
	}
	return EC_SUCCESS;
}

enum ErrorCode thread_GetNextAvailRes(DocID* p_doc_id,unsigned int* p_num_res,QueryID** p_query_ids)
{
	int i=0;
	sem_wait(semaphorResults);//wait until there there is at least one ready document//
	while (i<BATCHSIZE) //itterate over the ReadyDocs array//
	{
		pthread_mutex_lock(&mutexReady);//lock the Global Array//
		if(!ReadyDocs[i])//doc reference was marked as ready//
		{
			ReadyDocs[i]=1;//mark the doc as returned//
			pthread_mutex_unlock(&mutexReady);//unlock the Global Array//
			*p_doc_id = Documents[i]->doc_id;//set the d_id//
			*p_num_res = Documents[i]->Queries->length;//set the length of the array//
			(*p_query_ids)=malloc(sizeof(QueryID)*Documents[i]->Queries->length);//allocate space of the array//
			RadixSortList(Documents[i]->Queries);//sort the data
			initit_list_int(Documents[i]->Queries);//init the itterator//
			int j;
			for (j=0;j<Documents[i]->Queries->length;j++)
			{
				(*p_query_ids)[j]=(QueryID)getit_list_int(Documents[i]->Queries);
				pushit_list_int(Documents[i]->Queries);//push the itterator//
			}
			DestroyDocument(Documents[i]);
			free(Documents[i]);
			Documents[i]=NULL;
			ReadyDocs[i]=-1;//init the ready docs possition (threadsafe since reading threads will not write to it because its previews value was 1 and matching threads will only write to -1 vals//
			return EC_SUCCESS;
		}
		else pthread_mutex_unlock(&mutexReady);//unlock the Global Array//
		i++;//inc the counter//
	}
	return EC_NO_AVAIL_RES;//case the thread found nothing//
}


enum ErrorCode thread_MatchDocument(DocID doc_id,const char*   doc_str,Cache* cache,int GlobArrPos)
{
	int i, j, **bitArray, max_rows;
	Document* doc=malloc(sizeof(Document));
	EntryList Entries_Thresh1, Entries_Thresh2, Entries_Thresh3, res_exact;
	EntryList Edit1,Edit2,Edit3,Ham1,Ham2,Ham3,ExEntries;
	list_int_stack tempList;
	
	CreateDocument(doc_id,doc_str,doc);//Create the document//
	RemoveDuplicates(doc);//remove the duplicates//
	Entry* entrypointer=NULL;
	//////////////
	CreateEntryList(&res_exact);
	CreateEntryList(&Edit1);
	CreateEntryList(&Edit2);
	CreateEntryList(&Edit3);
	CreateEntryList(&Ham1);
	CreateEntryList(&Ham2);
	CreateEntryList(&Ham3);
	for(i=0;i<doc->length;i++){
        cache_entry *CE;
        if((CE=cache_lookup(cache,&(doc->words[i])))!=NULL){
            //word is in cache
            copyEntryList(&res_exact,&(CE->ExResult));
            copyEntryList(&Edit1,&(CE->EditRes1));
            copyEntryList(&Edit2,&(CE->EditRes2));
            copyEntryList(&Edit3,&(CE->EditRes3));
            copyEntryList(&Ham1,&(CE->HamRes1));
            copyEntryList(&Ham2,&(CE->HamRes2));
            copyEntryList(&Ham3,&(CE->HamRes3));
        }
        else{
            //we need to do lookups on idexes
            CE = malloc(sizeof(cache_entry));//to save word results and store in cache
            init_cache_entry(CE,&(doc->words[i]));
            //////////
            //lookup for exact
            entrypointer=find_Hashmap_SC_BT_Entry(ExactIndex,&(doc->words[i]));//returns the Entry address or NULL
            if(entrypointer)//found something//
                AddEntry(&(CE->ExResult),entrypointer);//copy the Entry to the exact results list//
            /////////
            for(j=1;j<=3;j++){//lookup for Edit and Hamming
                int wlength = doc->words[i].length;
                if(j==1){
                    LookupEntryIndex(&doc->words[i],1,&EditIndex,&(CE->EditRes1));
                    LookupEntryIndex(&doc->words[i],1,HamStruct.HamIndex[wlength-MIN_WORD_LENGTH],&(CE->HamRes1));
                }
                else if(j==2){
                    LookupEntryIndex(&doc->words[i],2,&EditIndex,&(CE->EditRes2));
                    LookupEntryIndex(&doc->words[i],2,HamStruct.HamIndex[wlength-MIN_WORD_LENGTH],&(CE->HamRes2));
                }
                else{//j==3
                    LookupEntryIndex(&doc->words[i],3,&EditIndex,&(CE->EditRes3));
                    LookupEntryIndex(&doc->words[i],3,HamStruct.HamIndex[wlength-MIN_WORD_LENGTH],&(CE->HamRes3));
                }
            }	
            //update result lists
            copyEntryList(&res_exact,&(CE->ExResult));
            copyEntryList(&Edit1,&(CE->EditRes1));
            copyEntryList(&Edit2,&(CE->EditRes2));
            copyEntryList(&Edit3,&(CE->EditRes3));
            copyEntryList(&Ham1,&(CE->HamRes1));
            copyEntryList(&Ham2,&(CE->HamRes2));
            copyEntryList(&Ham3,&(CE->HamRes3));
            //add to cache
            add_to_cache(CE,cache);//each thread has its own cache so there is no need for mut exclusion//
        }
	}
	max_rows = num_hamm;
	if(num_exact>max_rows)
        max_rows = num_exact;
    if(num_edit>max_rows)
        max_rows = num_edit;
    //allocate space for bitMap
    bitArray = malloc(max_rows*sizeof(int*));
    for(i=0;i<max_rows;i++)
        bitArray[i] = malloc(5*sizeof(int));
    zerofy(bitArray,max_rows);//fill it with zeros
    /*FINDING FINAL RESULTS*/
    init_list_int(&tempList);//initialize list that holds results
    /*Result Queries for Exact*/
    fillMap(&res_exact,bitArray);
    for(i=0;i<num_exact;i++){
        int sum=0;
        for(j=0;j<5;j++)
            sum+=bitArray[i][j];
        if(sum==ExactQueries[i].n_words)
            add_list_int(&tempList,ExactQueries[i].q_id);
    }
    zerofy(bitArray,num_exact);
    ///////////////////////////
    /*Result Queries for Edit*/
    fillMap(&Edit1,bitArray);//Threshold 1
    addResults(bitArray,&tempList,MT_EDIT_DIST,1);
    zerofy(bitArray,num_edit);
    fillMap(&Edit2,bitArray);//Threshold 2
    addResults(bitArray,&tempList,MT_EDIT_DIST,2);
    zerofy(bitArray,num_edit);
    fillMap(&Edit3,bitArray);//Threshold 3
    addResults(bitArray,&tempList,MT_EDIT_DIST,3);
    zerofy(bitArray,num_edit);
    //////////////////////////
    /*Result Queries for hamming*/
    fillMap(&Ham1,bitArray);//Threshold 1
    addResults(bitArray,&tempList,MT_HAMMING_DIST,1);
    zerofy(bitArray,num_hamm);
    fillMap(&Ham2,bitArray);//Threshold 2
    addResults(bitArray,&tempList,MT_HAMMING_DIST,2);
    zerofy(bitArray,num_hamm);
    fillMap(&Ham3,bitArray);//Threshold 3
    addResults(bitArray,&tempList,MT_HAMMING_DIST,3);
	//////////////////////////////////////////
	doc->Queries = copy_list_int(&tempList);
	Documents[GlobArrPos]=doc;//write the result on the global array (each job is given the index in global array to write so no need for mut excl)//
	ReadyDocs[GlobArrPos]=0;//no need for mutex , the getval thread will only write this if its value is 0(its previews now is -1)//
	sem_post(semaphorResults);//post the results sem so a get next thread can continue//
	//////////////////////////////////////////////////////////
	/*DELETING STUFF*/
	for(i=0;i<max_rows;i++)
		free(bitArray[i]);
	free(bitArray);
	DestroyEntryList(&Edit1);
	DestroyEntryList(&Edit2);
	DestroyEntryList(&Edit3);
	DestroyEntryList(&res_exact);
	DestroyEntryList(&Ham1);
	DestroyEntryList(&Ham2);
	DestroyEntryList(&Ham3);
	delete_list_int(&tempList);
	
	return EC_SUCCESS;
}

void fillMap(EntryList *Entries_Threshn, int **bitMap){
    Entry *temp;
    node_int_stack* temp2;
    int i,j;
    temp = GetFirst(Entries_Threshn);
    for(i=0;i<GetNumberOfEntries(Entries_Threshn);i++){//for each Entry
        initit_list_int(temp->payload->queryPos_list);
        for(j=0;j<temp->payload->queryPos_list->length;j++){//for each node of payload list
            temp2 = get_ints_pointer(temp->payload->queryPos_list);
            bitMap[temp2->data][temp2->data2] = 1;
            pushit_list_int(temp->payload->queryPos_list);
        }
        temp = GetNext(Entries_Threshn);
    }
}


void addResults(int **bitMap,list_int_stack *results,enum MatchType MT, int threshold){
    int i, j, sum, array_size;
    if(MT == MT_EDIT_DIST)
        array_size = num_edit;
    else if(MT == MT_HAMMING_DIST)
        array_size = num_hamm;
    for(i=0;i<array_size;i++){
        sum = 0;
        for(j=0;j<5;j++)
            sum+=bitMap[i][j];
        if(MT == MT_EDIT_DIST){
            if(sum==EditQueries[i].n_words && EditQueries[i].dist==threshold)//if all words satisfied and thresh==dist
                add_list_int(results,EditQueries[i].q_id);
        }
        else if(MT == MT_HAMMING_DIST){
            if(sum==HammingQueries[i].n_words && HammingQueries[i].dist==threshold)//if all words satisfied and thresh==dist
                add_list_int(results,HammingQueries[i].q_id);
        }
    }
}

void zerofy(int **bitMap, int rows){
    int i, j;
    for(i=0;i<rows;i++)
		for(j=0;j<5;j++){
			bitMap[i][j] = 0;
		}
}


/*********IMPLEMENTATION OF A JOB TAIL FOR FIFO****************/
/***The implementation is designed to access a FIFO tail*******/
/***and contains routines that minimize the amount of ops *****/
/**on the FIFO so the Critical Section on the FIFO is as small*/
/**as possible*************************************************/
/**************************************************************/

tail_node* create_tailnode(Job* job)
{
	tail_node* node;
	node = malloc(sizeof(tail_node));
	node->previews=NULL;
	node->data = job;
	return node;
}

Job* destroy_tailnode(tail_node* node)
{
	if(!node)return NULL;
	Job* temp;
	temp=node->data;
	free(node);
	return temp;
}

void push_tailnode(JobsTail* tail,tail_node* node)
{
	if(tail->length)
	{
		node->next =tail->head;
		tail->head->previews=node;
		tail->head = node;
	}
	else
	{
		node->next=NULL;
		tail->tail=node;
		tail->head=node;
	}
	tail->length++;
}

tail_node* pop_tailnode(JobsTail* tail)
{
	if(tail->length>1)
	{
		tail_node* temp=tail->tail; 
		tail->tail=tail->tail->previews;
		tail->tail->next=NULL;
		tail->length--;
		return temp;
	}
	else if(tail->length>0)
	{
		tail_node* temp = tail->tail;
		tail->tail=NULL;
		tail->head=NULL;
		tail->length--;
		return temp;
	}
	return NULL;
}

void init_tail(JobsTail* tail)
{
	tail->length=0;
	tail->head=NULL;
	tail->tail=NULL;
}

void push_tail(JobsTail* tail,Job* job)
{
	tail_node* temp=malloc(sizeof(tail_node));
	temp->data = job;
	temp->next = tail->head;
	temp->previews = NULL;
	tail->head= temp;
	if(tail->length)
	{
		tail->head->next->previews=tail->head;
	}
	if(!tail->length)
		tail->tail=tail->head;
	tail->length++;
}

Job* pop_tail(JobsTail* tail)
{
	if(tail->tail)
	{
		Job* temp;
		tail_node* node_ptr;
		node_ptr = tail->tail;
		tail->tail=tail->tail->previews;
		if(tail->tail)
			tail->tail->next=NULL;
		temp=node_ptr->data;
		free(node_ptr);
		tail->length--;
		if(!tail->length)
			tail->head=NULL;
		return temp;
		
	}
	return NULL;
}

void print_tail(JobsTail* tail)
{
	int i;
	printf("tail length is %d\n",tail->length);
	printf("headadd = %p ,tailadd =%p\n",tail->head,tail->tail);
	if((!tail->head)||(!tail->tail))
	{	
		printf("\n");
		return;
	}
	printf("head = %p,tail = %p\n",tail->head->data,tail->tail->data);
	tail_node* pointer = tail->head;
	for(i=0;i<tail->length;i++)
	{
		printf("(%p)->",pointer->data);
		pointer=pointer->next;
	}
	printf("\n");
}
/**************************************/
