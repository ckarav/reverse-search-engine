#include "core.h"
#include "data_struct.h"
#include "defn_impl.h"
#include "defn.h"
#include "index.h"
#include "index_impl.h"
#include "data_sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "threads.h"
#include "cache.h"

#define EXINSIZE 100

HashMap_SC_BTEntry* ExactIndex=NULL;
HammingStruct HamStruct;
Index EditIndex;

void* thread_ExecJob(void* cache);

Cache* Caches[THREADSNUM+1];
JobsTail FIFO;
pthread_mutex_t mutexFIFO;
pthread_mutex_t mutexReady;
sem_t* semaphorFIFO;
sem_t* semaphorResults;
pthread_t Threads[THREADSNUM];
pthread_barrier_t Barrier;

Query* ExactQueries=NULL; 
Query* HammingQueries=NULL;
Query* EditQueries=NULL;
int num_exact=0;
int num_hamm=0;
int num_edit=0;
Document* Documents[BATCHSIZE];
int AnsweredDocIDs[BATCHSIZE];//Array with the results from the GetNext Jobs//
int AnsweredDocArSize[BATCHSIZE];//Array with the results from the GetNext Jobs//
QueryID* AnsweredDocQueries[BATCHSIZE];//Array with the resutls from the GetNext Jobs//
int num_docs=0;
int num_matcalls=0;
int ReadyDocs[BATCHSIZE];
int thread_matchCalls=0;
char** map_queries=NULL;
int master_is_slave=0;

enum ErrorCode InitializeIndex() {
	int i;
	ExactIndex = malloc(sizeof(HashMap_SC_BTEntry));
	if(!ExactIndex)
		return EC_FAIL;
	init_HashMap_SC_BTEntry(ExactIndex,EXINSIZE);//[@temp]must check for false malloc
	//init EditStruct
	EditIndex.root = NULL;
	//init HammingStruct
	for(i=0;i<MAX_WORD_LENGTH-MIN_WORD_LENGTH+1;i++)
		HamStruct.HamIndex[i] = NULL;
	//initialize the global arrays//

	ExactQueries = malloc(0);
	HammingQueries =malloc(0);
	EditQueries=malloc(0);
	if((!ExactQueries)||(!HammingQueries)||(!EditQueries))//dummie pointers where not initialized//
		return EC_FAIL;
	for(i=0;i<BATCHSIZE;i++)
		ReadyDocs[i]=-1;
	for(i=0;i<THREADSNUM+1;i++)
	{
		Caches[i]=malloc(sizeof(Cache));
		init_cache(Caches[i]);
	}
	return EC_SUCCESS;
}

enum ErrorCode DestroyIndex() {
	int i;
	for(i=0;i<THREADSNUM;i++)
	{
		Job *temp = malloc(sizeof(Job));
	        temp->type=EXIT;
		push_tail(&FIFO,temp);
		sem_post(semaphorFIFO);
	}
	for(i=0;i<THREADSNUM;i++)
	{
		pthread_join(Threads[i],NULL);
	}
	if(!ExactIndex)//exact index was either not allocated or overwriten with NULL value
		return EC_FAIL;
	delete_HashMap_SC_BTEntry(ExactIndex);
	free(ExactIndex);
	DestroyHammingStruct(&HamStruct);//destroy Hamming Indexes
	DestroyEntryIndex(&EditIndex);//destroy Edit Index
	pthread_mutex_destroy(&mutexFIFO);
	pthread_mutex_destroy(&mutexReady);
	pthread_barrier_destroy(&Barrier);
	
	free(semaphorFIFO);
	free(semaphorResults);

	for(i=0;i<num_exact;i++)
		DestroyQuery(&ExactQueries[i]);
	for(i=0;i<num_hamm;i++)
		DestroyQuery(&HammingQueries[i]);
	for(i=0;i<num_edit;i++)
		DestroyQuery(&EditQueries[i]);
	free(ExactQueries);
	free(HammingQueries);
	free(EditQueries);
	for(i=0;i<THREADSNUM+1;i++)
		destroy_cache(Caches[i]);
	return EC_SUCCESS;
}

enum ErrorCode StartQuery(QueryID             query_id,
                          const char*         query_str,
                          enum MatchType      match_type,
                          unsigned int        match_dist) {
	Query query;
	CreateQuery(query_id,query_str,match_type,match_dist,&query);//create the query//
	if(match_type==MT_EXACT_MATCH)//in case of exact we can add the query to the Exact Index//
	{
		int i;
		Entry *entry;
		ExactQueries=realloc(ExactQueries,sizeof(Query)*(num_exact+1));//create space for the query on the query array//
		ExactQueries[num_exact]=query;//add the query//
		num_exact++;//increment the exact array size index//
		//add the query to the ExactIndex//
		for(i=0;i<query.n_words;i++)//itterate over the words of the query//
		{
		    entry = malloc(sizeof(Entry));
			CreateEntry(&query.wordz[i],entry);//create an entry copycat of the query word//
			add_list_int2(entry->payload->queryPos_list,num_exact-1,i);//initialize its payload with the query id//
			add_HashMap_SC_BTEntry(ExactIndex,entry);//add the query to the exact index//
			//(duplicates will not be added but their q_id lists will be concated with the existing entry's q_id list//
			if(!entry->word->length){//was marked as duplicate//
				DestroyEntry(entry);free(entry);}//destroy the entry since we dont keep any reference to it out of the loop//
		}
	}
	else if(match_type==MT_EDIT_DIST)//in case of edit distance we must add the queries alltogether to the EditIndex on the first call of match document//
	{
		EditQueries=realloc(EditQueries,sizeof(Query)*(num_edit+1));//create space for the query on the query array//
		EditQueries[num_edit]=query;//add the query//
		num_edit++;//increment the edit array size index//
	}
	else//in case of hamming distance we must add the queries to the HammingIndex alltogether on the fitsy call of match document//
	{
		HammingQueries=realloc(HammingQueries,sizeof(Query)*(num_hamm+1));//create space for the query on the query array//
		HammingQueries[num_hamm]=query;//add the query//
		num_hamm++;//increment the hamm array size index//
	}
	return EC_SUCCESS;
}

int match_once=0;//global array to ensure the MatchDoc is called only once//

enum ErrorCode MatchDocument(DocID         doc_id,
                             const char*   doc_str) {

	if(!match_once)//first call of the MatchDocument routine//
	{
		match_once=1;
		MASTER = pthread_self();
		int i,j;
		EntryList edit,hamming;
		CreateEntryList(&edit);
		CreateEntryList(&hamming);
		HashMap_SC_BTEntry editHash;//hashmap for deduplication//
		HashMap_SC_BTEntry hammingHash;//hashmap for deduplication//
		init_HashMap_SC_BTEntry(&editHash,num_edit*5);//create a hash with length as the max words in the edit querries//
		init_HashMap_SC_BTEntry(&hammingHash,num_hamm*5);//create a hash with length as the max words in the hamming querries//
		/****DEDUPLICATION OF QUERIES FOR INDEXES THAT USE VPTREES*****/
		//for edit//
		for(i=0;i<num_edit;i++)//put all the query entries into one list//itterate over the queries//
		{
			for(j=0;j<EditQueries[i].n_words;j++)//itterrate over the query words//
			{
				Entry* temp=malloc(sizeof(Entry));//allocate size for new entry//
				CreateEntry(&EditQueries[i].wordz[j],temp);//create the new entry//
				add_list_int2(temp->payload->queryPos_list,i,j);//init the payload//
				AddEntry(&edit,temp);//add the Entry to the edit Entrylist//
			}
		}
		RemoveDuplicatesEntryList(&edit);//remove the duplicates//
		//for hamming//
		for(i=0;i<num_hamm;i++)//itterate over the hamming queries//
		{
			for(j=0;j<HammingQueries[i].n_words;j++)//itterate over the words of the query//
			{
				Entry* temp=malloc(sizeof(Entry));//allocate memory for the entry//
				CreateEntry(&HammingQueries[i].wordz[j],temp);//create the new entry//
				add_list_int2(temp->payload->queryPos_list,i,j);//init the payload//
				AddEntry(&hamming,temp);//add the entry to the list//
			}
		}
		RemoveDuplicatesEntryList(&hamming);//remove the duplicates//
		/********************VP TREES BUILD*************************************/
		//Code for building the VPTREE Indexes with the following structures://
		BuildEntryIndex(&edit,MT_EDIT_DIST,&EditIndex);//build Edit Index
		BuildHammingStruct(&hamming,&HamStruct);//build Hamming Indexes
		//destroy the 2 lists
		DestroyEntryList(&edit);
		DestroyEntryList(&hamming);
		/*****INITIALIZE SEMAPHORS*******/
		pthread_mutex_init(&mutexFIFO,NULL);
		pthread_mutex_init(&mutexReady,NULL);
		/*****INITIALIZE BARRIER**********/
		pthread_barrier_init(&Barrier,NULL,THREADSNUM+1);
		///////////////////
		semaphorFIFO=malloc(sizeof(sem_t));
		semaphorResults=malloc(sizeof(sem_t));
		sem_init(semaphorFIFO,0,0);
		sem_init(semaphorResults,0,0);
		/****INITIALIZE GLOBAL FIFO******/
		init_tail(&FIFO);
		/************SPAWN THREADS**************************/
		for(i=0;i<THREADSNUM;i++)
		{
			pthread_create(&Threads[i],NULL,thread_ExecJob,Caches[i]);
		}
	}
	master_is_slave=1;//the next call to GetNext will be consedered the first
	//add match jobs to the FIFO//
	Job* temp=malloc(sizeof(Job));
	temp->JobParams = malloc(sizeof(MatchDocParams));
	((MatchDocParams*)(temp->JobParams))->doc_id = doc_id;
	((MatchDocParams*)(temp->JobParams))->str=malloc(sizeof(char)*(strlen(doc_str)+1));
	strcpy(((MatchDocParams*)(temp->JobParams))->str,doc_str);
  	((MatchDocParams*)(temp->JobParams))->GlobArrPos = num_matcalls;
	temp->type = MATCH_DOC;
	tail_node* nodeptr = create_tailnode(temp);
	pthread_mutex_lock(&mutexFIFO);
	push_tailnode(&FIFO,nodeptr);
	pthread_mutex_unlock(&mutexFIFO);
	sem_post(semaphorFIFO);
	num_matcalls++;//count the num of matchdoc calls so the scheduler can push the same amount of getresult jobs//
	
	return EC_SUCCESS;
}


int call_count=0; //global counter of GetNext calls//
enum ErrorCode GetNextAvailRes(DocID*         p_doc_id,
                               unsigned int*  p_num_res,
                               QueryID**      p_query_ids) {
	int i;
	if(master_is_slave)//first call of get_next//
	{
		call_count=0;
		master_is_slave=0;
		for(i=0;i<num_matcalls;i++)//add GetNext Jobs to FIFO//
		{
			Job* temp=malloc(sizeof(Job));
			temp->type = GET_RESULT;
			temp->JobParams=malloc(sizeof(GetNextParams));
			((GetNextParams*)(temp->JobParams))->doc_id = &AnsweredDocIDs[i];//the results will be writen on these global arrays 
			((GetNextParams*)(temp->JobParams))->p_num_res=&AnsweredDocArSize[i];
			((GetNextParams*)(temp->JobParams))->p_query_ids=&AnsweredDocQueries[i];
			tail_node* nodeptr = create_tailnode(temp);
			pthread_mutex_lock(&mutexFIFO);
			push_tailnode(&FIFO,nodeptr);
			pthread_mutex_unlock(&mutexFIFO);
			sem_post(semaphorFIFO); //post the FIFO semaphor//
		}
		num_matcalls=0;
		for(i=0;i<THREADSNUM+1;i++)//add Barrier Jobs , one for each thread (including the master)//
		{
			Job* temp = malloc(sizeof(Job));
			temp->type=BARRIER;
			tail_node* nodeptr = create_tailnode(temp);
			pthread_mutex_lock(&mutexFIFO);
			push_tailnode(&FIFO,nodeptr);
			pthread_mutex_unlock(&mutexFIFO);
			sem_post(semaphorFIFO);
		}
		thread_ExecJob(Caches[THREADSNUM]);//the scheduler has finished, and now will work as a worker//
	}
	*p_doc_id =AnsweredDocIDs[call_count]; //write the results from the global arrays (all results are filled at this point)
	*p_num_res = AnsweredDocArSize[call_count];
	*p_query_ids = AnsweredDocQueries[call_count];
	call_count++; //inc the get_next call counter//
	
	return EC_SUCCESS;
}









