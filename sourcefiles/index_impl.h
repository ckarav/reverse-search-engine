#ifndef INDEX_IMPL_H
#define	INDEX_IMPL_H

#include "index.h"

#define MAXIMUM_HEIGHT 20

#ifdef	__cplusplus
extern "C" {
#endif
	
typedef struct IndexNode IndexNode;
	
struct IndexNode{
		Entry *vp;//vantage point
		EntryList *points;//rest of entries
		IndexNode *left;//left subtree
		IndexNode *right;//right subtree
		int median;
};
	
struct Index {
	IndexNode *root;
	enum MatchType index_type;
};
	
struct HammingStruct{
	Index *HamIndex[MAX_WORD_LENGTH-MIN_WORD_LENGTH+1];//potential edit Index for each possible word length
};
	
typedef struct EntryNode EntryNode;
	
struct EntryNode
{
	EntryNode* next;
	Entry* data;
};
	
struct EntryList{
	EntryNode* head;
	EntryNode* tail;
	EntryNode* itterator;
	int length;
};
	
void printEntryList(EntryList* list);//debug function//
	
int find_vp(EntryList *entries,Entry **vp,enum MatchType type);//choose vantage point
void expandTree(IndexNode *node,EntryList *entries,int height,enum MatchType type);//create the tree, one node at a time
void destroyTree(IndexNode *node);
void find_words(IndexNode *node,const Word *word,int threshold,EntryList *result,enum MatchType type);//traverse the tree and get results
void RemoveDuplicatesEntryList(EntryList* list);//merge entries with same word value to one entry (concat their payload list)//
void PrintEntryList(EntryList* list); //for debug//
void copyEntryList(EntryList *L1, EntryList *L2);	
#ifdef	__cplusplus
}
#endif

#endif	/* INDEX_IMPL_H */

