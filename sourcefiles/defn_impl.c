/****************************************************************************************************/
/*list of tags:@typ,@dang****************************************************************************/
/****************************************************************************************************/

#include "defn.h"
#include "defn_impl.h"
#include "hash.h"
#include "data_struct.h"
#include "index_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PrintDocument(Document doc)
{
	int i=0;
	printf("Doc with id %d has length:%d\n",doc.doc_id,doc.length);
	printf("Doc with id %d contains these words:\n|",doc.doc_id);
	for(i=0;i<doc.length;i++)
	{
		if(doc.words){
			{
				printf("%s|",doc.words[i].characters);
			}

		}
	}
	printf("\n");
}

void CreateWord(const char* word_str,Word* word) {


	word->length = strlen(word_str);       //get str length and initialize struct length//
	word->characters = malloc((word->length+1)*sizeof(char));  //allocate mem for the struct string//
	strcpy(word->characters,word_str);
	word->characters[word->length]='\0';//copy const str to struct str//
	word->hash = M_HashCode(*word);  //get hashcode for this word//
}

void DestroyWord(Word* word)
{
	if(word->characters)
	{
		free(word->characters);
		word->characters =NULL;
	}
}

void CreateQuery(QueryID query_id,
                 const char* query_str,
                 enum MatchType match_type,
                 unsigned int match_dist,
                 Query* query) {
	char temp[32];//to extract each query word
	int i=0,j,n=0;
	query->q_id = query_id;
	query->n_words = 0;//tarzan
	query->type = match_type;
	query->dist = match_dist;
	while(*(query_str+i)!='\0')
	{
		j=0;
		while(*(query_str+i)!='\0' && *(query_str+i)!=' ')
		{
			temp[j] = *(query_str+i);
			j++;
			i++;
		}
		temp[j] = '\0';
		CreateWord(temp,&(query->wordz[n]));
		n++;
		if(*(query_str+i)==' ')
			i++;
	}
	query->n_words = n;
}

void DestroyQuery(Query* Q)//
{
	int i;
	for(i=0;i<Q->n_words;i++)
		DestroyWord(&(Q->wordz[i]));	
}

int GetNumQueryWords(const struct Query* query) {
    return query->n_words;
}

const struct Word* getQueryWord(unsigned int word, const  Query* query) {
	return &query->wordz[word];
}

void CreateDocument(DocID doc_id,const char* doc_str,Document* document) {

	int length = strlen(doc_str);//lenght of doc_str//
	int i=0;
	char* token;				//token var //
	list_Words word_list;	//a temporary stack for the tokens//
	char* doc_str_alias = malloc((length+1)*sizeof(char));//allocate new str//
	document->doc_id = doc_id;		//initialize document doc_id with param doc_id//
	init_list_Words(&word_list);
	strcpy(doc_str_alias,doc_str);
	char* save_ptr;
	token = strtok_r(doc_str_alias," ",&save_ptr);		//get first token of doc//
	while (token!=NULL)
	{
		Word* temp;
		temp = malloc(sizeof(Word));
		CreateWord(token,temp);				//Create the Word//
		add_list_Words(&word_list,temp); //add the word to the temporary stack//
		token = strtok_r(NULL," ",&save_ptr);		//get the next token of doc_str//
	}
	//free(doc_str_alias);
	
	document->length=word_list.length;	//init the doc length//
	document->words = malloc(sizeof(Word)*word_list.length); //alloc doc->length array of word*//
	//printf("doc has length %d\n",word_list.length);
	initit_list_Words(&word_list);		//init the list itterator to point at list head//
	for(i=0;i<word_list.length;i++)
	{
		CreateWord(getit_list_Words(&word_list)->characters,&document->words[i]);
		pushit_list_Words(&word_list);						//push the itterator to point to the next node//
	}
	destroy_list_Words(&word_list); //dealloc the list struct and its context//
	document->Queries =NULL;
	free(doc_str_alias);
	//printf("ok\n");
}

void DestroyDocument(Document* doc)
{
	int i;
	//printf("par1\n");
	for(i=0;i<doc->length;i++)
	{
		DestroyWord(&doc->words[i]);//[@temp]
	}
	//printf("par2\n");
	free(doc->words);
	//printf("par3\n");
	delete_list_int(doc->Queries);
	//printf("par4\n");
	free(doc->Queries);
	//printf("par5\n");
	doc->words=NULL;
}

int GetNumDocumentWords(const struct Document* doc) {
	return doc->length;
}

const struct Word* getDocumentWord(unsigned int word, const Document* doc) {
	return &doc->words[word];
}

int Equal(const Word* w1, const Word* w2) {
	if(!strcmp(w1->characters,w2->characters))
	{
        return 0;
	}
    return 1;
}

int HammingDistance(const Word* w1, const Word* w2) {
	char *temp1, *temp2;
    int dif = 0;
    temp1 = w1->characters;
    temp2 = w2->characters;
    if(w1->length!=w2->length)
	{
        return -1;
	}
    while(*temp1 != '\0')
    {
        if(*temp1 != *temp2)
            dif++;
        temp1++;
        temp2++;
    }
    return dif;
}

int EditDistance(const Word* w1, const Word* w2) {
	int **DPArray;
    int i, j, dist, u, l, ul;
    //allocate space for array
    DPArray = malloc((w1->length+1)*sizeof(int *));
    for(i=0;i<w1->length+1;i++)
        *(DPArray+i) = malloc((w2->length+1)*sizeof(int));
    //initialize
    for(i=0;i<w2->length+1;i++)
        DPArray[0][i]=i;
    for(i=0;i<w1->length+1;i++)
        DPArray[i][0]=i;
    //////////////////
    for(j=1;j<w2->length+1;j++)
    {
        for(i=1;i<w1->length+1;i++)
        {
            int min;
            u = DPArray[i-1][j]+1;
            l = DPArray[i][j-1]+1;
            ul = DPArray[i-1][j-1];
            if(*(w1->characters+(i-1)) != *(w2->characters+(j-1)))
                ul++;
            min = u;
            if(l<min)
                min = l;
            if(ul<min)
                min = ul;
            DPArray[i][j] = min;
        }
    }
    //////////////////
    dist = DPArray[w1->length][w2->length];
    //free array
    for(i=0;i<w1->length+1;i++)
        free(*(DPArray+i));
    free(DPArray);
    return dist;
}

void RemoveDuplicates(struct Document* doc) {

	/*HASHMAP IMPLEMENTATION (MULT HASH FUNCTION with SEPARATE CHAINING BINARY TREES)*/
	int i=0;
	int j=0;
	HashMap_SC_BT hashmap; //hashmap that does not add duplicates//
	Word* new_words;   //new word array//
	init_HashMap_SC_BT(&hashmap,doc->length+(!(doc->length%2))); //initialize the hashmap and alloc the num of words(always odd)//@dang odd num hack for all complrs?
	for(i=0;i<doc->length;i++)
	{
		add_HashMap_SC_BT(&hashmap,&doc->words[i]);	//[@temp]add all the words of the array to the hashmap (duplicates will not be added,and will be marked as empty strs)//
	}
	new_words = malloc(hashmap.num_of_words*sizeof(Word)); //allocate the new array by the num_of_words of the hashmap(duplicates are added on the hashmap)//
	for(i=0;i<doc->length;i++)
	{
		if(doc->words[i].length)	//this word is original and it should be copied on the new array of words)//
		{
			CreateWord(doc->words[i].characters,&new_words[j]);
			j++;
		}
		DestroyWord(&doc->words[i]);//delete the old word//
	}
	free(doc->words);  //dealoc the old array of words//
	doc->words = new_words; //connect the new word array to the doc word pointer//
	doc->length = hashmap.num_of_words; //update the length of the doc by the num of worlds on the hashmap (duplicates are not counted or added on the hash)//
	delete_HashMap_SC_BT(&hashmap);  //delete the hashmap struct//
}
