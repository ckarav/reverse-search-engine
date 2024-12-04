/*****************************/
/****List of tags[arg,temp]***/
/*****************************/

#include <stdlib.h>
#include <stdio.h>

#include "core.h"
#include "index.h"
#include "defn_impl.h"
#include "index_impl.h"
#include "data_sort.h"
#include "data_struct.h"
#include "primitive_data_struct.h"

void copyEntryList(EntryList *L1, EntryList *L2){
    Entry *temp;
    int i;
    temp = GetFirst(L2);
    for(i=0;i<GetNumberOfEntries(L2);i++){
        AddEntry(L1,temp);
        temp = GetNext(L2);
    }
}

enum ErrorCode CreateEntry(const Word* word, Entry* entry) {

	entry->word = malloc(sizeof(Word)); //allocate mem for word
	CreateWord(word->characters,entry->word); //create word
	entry->payload=malloc(sizeof(Payload));
	/////////////////////////////
	//maybe remove?
	entry->payload->queryPos_list=malloc(sizeof(list_int_stack));//allocate space
	init_list_int(entry->payload->queryPos_list);//list of query pos in array//
	//////////////////////////////////
	return EC_SUCCESS;
}

enum ErrorCode AddEntry(EntryList* entryList, const Entry* entry) {

	if(entryList->length) //case size of list is greater than zero//
	{
		list_int_stack* templistpointer;
		entryList->tail->next = malloc(sizeof(EntryNode));//allocate mem for the new node//
		entryList->tail->next->data=malloc(sizeof(Entry));//allocate mem for the new node data//
		CreateEntry(entry->word,entryList->tail->next->data); //create new node data//
		//DestroyEntry(entry);
		///////
		free(entryList->tail->next->data->payload->queryPos_list);
		/////
		templistpointer=copy_list_int(entry->payload->queryPos_list);//data copy list
		entryList->tail->next->data->payload->queryPos_list=templistpointer;//connect list
		entryList->tail =entryList->tail->next; //point the tail pointer to the new node//
		entryList->tail->next=NULL; //set the "next" value of the new node to NULL//
		//DestroyEntry(entry);
	}
	else
	{
		list_int_stack* templistpointer;
		entryList->head = entryList->tail = malloc(sizeof(EntryNode)); //allocate mem for the new node and point both tail and head to it//
		entryList->head->data=malloc(sizeof(Entry));//allocate mem for new node data//
		CreateEntry(entry->word,entryList->head->data);//create new node data//
		//DestroyEntry(entry);
		/////////
		free(entryList->head->data->payload->queryPos_list);
		/////////
		templistpointer=copy_list_int(entry->payload->queryPos_list);//data copy list
		entryList->head->data->payload->queryPos_list=templistpointer;//connect list//
		entryList->head->next=NULL;//set new node "next" value to NULL//
		//DestroyEntry(entry);
	}
	entryList->length++; //update the list length info//
	return EC_SUCCESS;
}


enum ErrorCode DestroyEntry(Entry *entry) {

	if(!entry)
		return EC_SUCCESS;
	DestroyWord(entry->word);
	free(entry->word);
	entry->word=NULL;
	delete_list_int(entry->payload->queryPos_list);
	free(entry->payload->queryPos_list);
	free(entry->payload);

	return EC_SUCCESS;
}

enum ErrorCode CreateEntryList(EntryList* entryList) {

	entryList->head =NULL; //initialize head pointer to NULL//
	entryList->tail=NULL; //initialize tail pointer to NULL//
	entryList->itterator =NULL; //initialize itterator to NULL (or head val)//
	entryList->length=0; //initialize list length to zero//
	return EC_SUCCESS;
}

Entry* GetFirst(EntryList* entryList) {
	if(entryList->length) //list has some data//
	{
		entryList->itterator = entryList->head;//initialize the itterator to point at the head of the list//
		return entryList->itterator->data; //return the data that the itterator points to//
	}
	return NULL; //empty list without any data//
}

Entry* GetNext(EntryList* entryList) {
	if(entryList->length)//list has some data//
	{
		entryList->itterator=entryList->itterator->next; //push the itterator to point to the next node//
		if(!entryList->itterator)						//case we reached the end of the list and we pushed further//
			entryList->itterator=entryList->head;       //[@temp]we handle the problem by circuling the itterator around the list//
		return entryList->itterator->data;				//return the data that the itterator points to//
	}
	return NULL; //empty list without any data//
}

enum ErrorCode DestroyEntryList(EntryList* entryList) {
	if(entryList->length)
	{
		int i;
		EntryNode* pointer=entryList->head; //pointer that points the node we destroy at each itteration//
		EntryNode* mem_pointer = pointer->next; //pointer that points the next node after the one we destroy//
		for(i=0;i<entryList->length-1;i++) //we itterate n-1 times
		{
			DestroyEntry(pointer->data); //destroy the node data//
			free(pointer);//destroy the node//
			pointer=mem_pointer;		//set the address of the pointer to be destroyed next//
			mem_pointer=mem_pointer->next; //update the helping pointer by pointing one node further//
		}
		DestroyEntry(pointer->data); //the nth itteration outside the FOR to avoid the helping pointer upadate (and thus the check for NULL in each itteration)//
		free(pointer);
		entryList->length = 0;//update of the length of the list,we can't destroy the argument here since we dont know if it was allocated dynamicaly or not//
		//so the length value might be use for the list again//
	}
	return EC_SUCCESS;
}

unsigned int GetNumberOfEntries(const EntryList* entryList) {
	return entryList->length; //return the length of the list//
}


int find_vp(EntryList *entries,Entry **vp,enum MatchType type){
    /*only EditDistance is currently being used to build the Index
	 (Hamming can easily be added if we know input words have same length)
	 */
    int length = GetNumberOfEntries(entries);
    int vp_candidates, second_sample;
    int i, j, max, **dist, *deviation, *average;
    EntryList vps_list, ssample_list;
    Entry *temp1, *temp2;
    /*calculate size of the two samples*/
    if(length<=3){//if 1/2/3 entries left, use first entry as vantage point
        *vp = GetFirst(entries);
        return 0;
    }
	if(length>20){
        vp_candidates = 5;
        second_sample = 15;
    }
    else if(length>15){
        vp_candidates = 5;
        second_sample = 10;
    }
    else{
        vp_candidates = 3;
        second_sample = length-3;
    }
    /*create the lists containing the samples*/
    CreateEntryList(&vps_list);
    CreateEntryList(&ssample_list);
    temp1 = GetFirst(entries);
    /*entries list is not sorted in any way and we can assume the Entries are "random"
	 so we take first "vp_candidates" for the possible vantage point set and the following
	 "second_sample" for the second random sample set
	 */
    for(i=0;i<vp_candidates;i++){
        AddEntry(&vps_list,temp1);
        temp1 = GetNext(entries);
    }
    //temp1 is already pointing at the first Entry of the second (sample) set
    for(i=0;i<second_sample;i++){
        AddEntry(&ssample_list,temp1);
        temp1 = GetNext(entries);
    }
    /*calculating distances and storing them in an array*/
    /*allocating space for array*/
    //each row is a possible vantage point and each column an Entry from the second sample
    dist = malloc(vp_candidates*sizeof(int *));
    for(i=0;i<vp_candidates;i++)
        dist[i] = malloc(second_sample*sizeof(int));
    temp1 = GetFirst(&vps_list);
    for(i=0;i<vp_candidates;i++){
		temp2 = GetFirst(&ssample_list);
        for(j=0;j<second_sample;j++){
			if(type==MT_HAMMING_DIST)
				dist[i][j] = HammingDistance(temp1->word,temp2->word);//calculate distance
			else if(type==MT_EDIT_DIST)
				dist[i][j] = EditDistance(temp1->word,temp2->word);//calculate distance
            temp2 = GetNext(&ssample_list);
        }
        temp1 = GetNext(&vps_list);
    }
    /*calculating standard deviation(almost)*/
    average = malloc(vp_candidates*sizeof(int));
    deviation = malloc(vp_candidates*sizeof(int));
    //first calculating average
    for(i=0;i<vp_candidates;i++){
        average[i] = 0;
        for(j=0;j<second_sample;j++)
            average[i] += dist[i][j];
        average[i] /= second_sample;
    }
    //calculate deviation
    for(i=0;i<vp_candidates;i++){
        deviation[i] = 0;
        for(j=0;j<second_sample;j++)
            deviation[i] += (dist[i][j]-average[i]) * (dist[i][j]-average[i]);
    }
    /*no need to calculate the full formula to find out which vp has the biggest deviation*/
    /*sqrt((S(Xi-avg1)^2)/N) > sqrt((S(Yi-avg2)^2)/N)==> S(Xi-avg1)^2 > S(Yi-avg2)^2*/
    /*finding max deviation*/
    max = 0;
    for(i=1;i<vp_candidates;i++){
        if(deviation[i]>deviation[max])
            max = i;
    }
    temp1 = GetFirst(entries);
    for(i=0;i<max;i++)
        temp1 = GetNext(entries);
    *vp = temp1;//*vp points to the vantage point
    ////////////////////////////
    /*free stuff*/
    for(i=0;i<vp_candidates;i++)
        free(dist[i]);
    free(dist);
    free(average);
    free(deviation);
    DestroyEntryList(&vps_list);
    DestroyEntryList(&ssample_list);
    return max;
}

void expandTree(IndexNode *node,EntryList *entries,int height,enum MatchType type){
    int vp_pos;//position of vp in entries list
    int *dist;//distance of vp to other entries;
    int numOfEntries = GetNumberOfEntries(entries);
    Entry *temp;
    int i, y;
    EntryList leftEntries, rightEntries;//new sets of "inner" and "outer" points
    //printf("Expansion is taking place\n");
    /*fisrt check if we reached maximum height*/
    if(height > MAXIMUM_HEIGHT){
        temp = GetFirst(entries);
        //copy the entries list in the points list
        node->points = malloc(sizeof(EntryList));
        CreateEntryList(node->points);
        for(i=0;i<numOfEntries;i++){
            AddEntry(node->points,temp);
            temp = GetNext(entries);
        }
        node->vp = NULL;
        node->left = NULL;
        node->right = NULL;
        node->median = -1;
        DestroyEntryList(entries);//destroy input list
        return;
    }
    /*find vp position and have pointer temp point at it*/
    vp_pos = find_vp(entries,&temp,type);
    node->vp = malloc(sizeof(Entry));//allocate space for new vp;
	CreateEntry(temp->word,node->vp); //[@arg]
	free(node->vp->payload->queryPos_list);//better?
	node->vp->payload->queryPos_list=copy_list_int(temp->payload->queryPos_list);//[@check]
    if(numOfEntries == 1){//case of just one point (vp)
        node->left = NULL;
        node->right = NULL;
        node->median = -1;
        node->points = NULL;
        DestroyEntryList(entries);
        return;
    }
    dist = malloc(sizeof(int)*(numOfEntries - 1));//allocate space for array with distances
    temp = GetFirst(entries);
    //calculate distances
    y = 0;
    for(i=0;i<numOfEntries;i++){//iterate list
        if(i==vp_pos){//exclude vp
            temp = GetNext(entries);
            continue;
        }
        if(type==MT_HAMMING_DIST)
			dist[y] = HammingDistance(node->vp->word,temp->word);
		else if(type==MT_EDIT_DIST)
			dist[y] = EditDistance(node->vp->word,temp->word);
        temp = GetNext(entries);
        y++;
    }
    //calculate median
    node->median = RadixSort(dist,numOfEntries-1);
    /////////////////////////////////////////////
    //DIVIDE INTO LEFT AND RIGHT SUBLIST//
    CreateEntryList(&leftEntries);
    CreateEntryList(&rightEntries);
    temp = GetFirst(entries);
    y = 0;
    for(i=0;i<numOfEntries;i++){//divide entries
        if(i==vp_pos){
            temp = GetNext(entries);
            continue;
        }
        if(dist[y] < (node->median))//all entries with distance < median go to the left subtree
            AddEntry(&leftEntries,temp);
        else
            AddEntry(&rightEntries,temp);//other entries(>= median) go to the right
        temp = GetNext(entries);
        y++;
    }
    //Destroy input list and free distance array
    DestroyEntryList(entries);//kid nodes destroy the parent-made list
    free(dist);
    height++;//increase height for next level
    if(GetNumberOfEntries(&leftEntries)){//if there are any points for left subtree
        node->left = malloc(sizeof(IndexNode));
        expandTree(node->left,&leftEntries,height,type);//expand left subtree
    }
    else
        node->left = NULL;
    if(GetNumberOfEntries(&rightEntries)){//if there are any points for right subtree
        node->right = malloc(sizeof(IndexNode));
        expandTree(node->right,&rightEntries,height,type);//expand right subtree
    }
    else
        node->right = NULL;
    node->points = NULL;//only vantage point in this node(haven't surpassed maximum height)
    ////////////////////
}



void destroyTree(IndexNode *node){

    if(node->vp){
        DestroyEntry(node->vp);
        free(node->vp);
    }
    if(node->points){
        DestroyEntryList(node->points);
        free(node->points);
    }
    if(node->left){
        destroyTree(node->left);
        free(node->left);
    }
    if(node->right){
		destroyTree(node->right);
        free(node->right);
    }
}

void find_words(IndexNode *node,const Word *word,int threshold,EntryList *result,enum MatchType type){
    int dist;
    Entry *temp;
    if(!node->vp){//case of points list containing the remaining entries
        int i;
	EntryNode* ptr=node->points->head;
	if(ptr)
	temp = ptr->data;
        while(ptr){
			if(type==MT_HAMMING_DIST)
				dist = HammingDistance(word,temp->word);
			else if(type==MT_EDIT_DIST)
				dist = EditDistance(word,temp->word);//get distance of each point in the list
            if(dist <=threshold)//if distance less than threshold add Entry to the result
              AddEntry(result,temp);
            ptr=ptr->next;
            if(ptr)
                temp=ptr->data;
        }
        return;
    }
    /*if not leaf of tree, check to see if vantage point should be added to the result*/
	if(type==MT_HAMMING_DIST)
		dist = HammingDistance(word,node->vp->word);
	else if(type==MT_EDIT_DIST)
		dist = EditDistance(word,node->vp->word);//[@arg]
    if(dist <= threshold)
	{
        AddEntry(result,node->vp);
	}
    /*checking subtrees*/
    if((dist <= threshold + node->median) && node->left)//if condition + there is a left subtree
        find_words(node->left,word,threshold,result,type);
    if((node->median <= threshold + dist) && node->right)//if condition + there is a right subtree
        find_words(node->right,word,threshold,result,type);
}



enum ErrorCode BuildEntryIndex(EntryList* entries,
                               enum MatchType type,
                               Index* index) {
    EntryList *elist;
    Entry *temp;
    int i;
    index->index_type = type;
    if(GetNumberOfEntries(entries)==0){//no entries to build index on
        index->root = NULL;
        return EC_SUCCESS;
    }
    index->root = malloc(sizeof(IndexNode));
    //copy entries to a new list
    elist = malloc(sizeof(EntryList));
    CreateEntryList(elist);
    temp = GetFirst(entries);
    for(i=0;i<GetNumberOfEntries(entries);i++){
        AddEntry(elist,temp);
        temp = GetNext(entries);
    }
    expandTree(index->root,elist,0,type);//call function with zero height
    DestroyEntryList(elist);
    free(elist);
    return EC_SUCCESS;
}

enum ErrorCode BuildHammingStruct(EntryList *entry_list, HammingStruct *HamStruct){
    //initially all HamIndexes are NULL
    EntryList *entries[MAX_WORD_LENGTH-MIN_WORD_LENGTH+1];
    Entry *temp;
    int i, length;
    for(i=0;i<MAX_WORD_LENGTH-MIN_WORD_LENGTH+1;i++)
        entries[i] = NULL;
    //constructing entry lists of same-size words
    temp = GetFirst(entry_list);
    for(i=0;i<GetNumberOfEntries(entry_list);i++){
        length = temp->word->length;
        /*if length-sized list has not yet been created*/
        if(entries[length-MIN_WORD_LENGTH]==NULL){
            entries[length-MIN_WORD_LENGTH] = malloc(sizeof(EntryList));
            CreateEntryList(entries[length-MIN_WORD_LENGTH]);
            AddEntry(entries[length-MIN_WORD_LENGTH],temp);
        }
        else
            AddEntry(entries[length-MIN_WORD_LENGTH],temp);
        temp = GetNext(entry_list);
    }
    /*Create all the same-sized Edit indexes*/
    for(i=0;i<MAX_WORD_LENGTH-MIN_WORD_LENGTH+1;i++){
        if(entries[i]!=NULL){
            //create i'th index
            HamStruct->HamIndex[i] = malloc(sizeof(Index));
            BuildEntryIndex(entries[i],MT_HAMMING_DIST,HamStruct->HamIndex[i]);
        }
    }
    /*Free allocated space*/
    for(i=0;i<MAX_WORD_LENGTH-MIN_WORD_LENGTH+1;i++){
        if(entries[i]!=NULL){
            DestroyEntryList(entries[i]);
            free(entries[i]);
        }
    }
    return EC_SUCCESS;
};

enum ErrorCode DestroyHammingStruct(HammingStruct *HamStruct){
    int i;
    for(i=0;i<MAX_WORD_LENGTH-MIN_WORD_LENGTH+1;i++){
        if(HamStruct->HamIndex[i]!=NULL){
            DestroyEntryIndex(HamStruct->HamIndex[i]);
            free(HamStruct->HamIndex[i]);
        }
    }
    return EC_SUCCESS;
};


enum ErrorCode DestroyEntryIndex(Index* index) {

	if(index==NULL)
		return EC_SUCCESS;
	if(index->root==NULL)
		return EC_SUCCESS;
    destroyTree(index->root);
	free(index->root);
    return EC_SUCCESS;
}

enum ErrorCode LookupEntryIndex(const Word* word,
                                int threshold,
                                const Index* index,
								EntryList* result) {
	if(index==NULL)
	{
		return EC_SUCCESS;
	}
    if(index->root!=NULL)//only if index is not empty
	{
			find_words(index->root,word,threshold,result,index->index_type);
	}
    return EC_SUCCESS;
}




void PrintEntryList(EntryList* list)
{
	int i=0;
	Entry* entrypointer;
	printf("List has Length %d\n",list->length);
	if(i<list->length)
	{
		entrypointer = GetFirst(list);
		printf("(%s,%p)->",entrypointer->word->characters,entrypointer);
		printf("its payload is:");
		print_list_int2(entrypointer->payload->queryPos_list);
		i++;
	}
	for(i=1;i<list->length;i++)
	{
		entrypointer = GetNext(list);
		printf("(%s,%p)->",entrypointer->word->characters,entrypointer);
		printf("its payload is:");
		print_list_int2(entrypointer->payload->queryPos_list);
	}
	printf("NULL\n");
}

void DeleteMarkedEntries(EntryList* list);
void RemoveDuplicatesEntryList(EntryList* list)
{
	int i;
	HashMap_SC_BTEntry hashmap;
	Entry* entrypointer;
	init_HashMap_SC_BTEntry(&hashmap,list->length);//initialize the hashmap//
	i=0;
	if(i<list->length)//to avoid the if in the for loop//
	{
		entrypointer = GetFirst(list);//get the first node//
		add_HashMap_SC_BTEntry(&hashmap,entrypointer);//mark the duplicates and copy their entrylist to the originals//
		i++;
	}
	for(i=1;i<list->length;i++)//itterate over the EntryList
	{
		entrypointer = GetNext(list);//get the next node//
		add_HashMap_SC_BTEntry(&hashmap,entrypointer);//mark the duplicates and copy their entrylist to the originals//
	}
	delete_HashMap_SC_BTEntry(&hashmap);//delete the hashmap since we dont need it anymore//
	DeleteMarkedEntries(list);//delete the duplicate entries makred by the hashmap//
}

void DeleteMarkedEntries(EntryList* list)
{
	int i;
	int new_length=list->length;
	EntryNode* nodepointer;
	EntryNode* previewspointer;
	nodepointer = list->head;
	previewspointer =list->head;
	for(i=0;i<list->length;i++)
	{
		if(!nodepointer->data->word->length)//was marked as duplicate//
		{
			DestroyEntry(nodepointer->data);//delete the data//
			free(nodepointer->data);//free the data pointer//
			nodepointer=nodepointer->next;//point to the next position//
			free(previewspointer->next);//the previews node will free the node//
			previewspointer->next=nodepointer;//previews node will point to current next as his next//
			//the previews pointer must still point to the same since its now the previews of the current//
			new_length--;
		}
		else
		{
			previewspointer=nodepointer;//the previews now points to the currentnode//
			nodepointer=nodepointer->next;//the current now points to the nextnode//
		}
	}
	list->length=new_length;
}


