#include "defn.h"
#include "defn_impl.h"
#include "index.h"
#include "index_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include "data_struct.h"
//for memleak tests// 

int main()
{
	//words//
/*	Word w1,w2,w3;
	Word *w4,*w5,*w6;
	w4=malloc(sizeof(Word));
	w5=malloc(sizeof(Word));
	w6=malloc(sizeof(Word));
	CreateWord("sdasda",&w1);
	CreateWord("sdasda",&w2);
	CreateWord("sdasda",&w3);
	CreateWord("sdasda",w4);
	CreateWord("sdasda",w5);
	CreateWord("sdasda",w6);
	DestroyWord(&w1);
	DestroyWord(&w2);
	DestroyWord(&w3);
	DestroyWord(w4);
	DestroyWord(w5);
	DestroyWord(w6);
//	free(w4);
//	free(w5);
//	free(w6);
	
	CreateWord("sdasda",&w1);
	CreateWord("sdasda",&w2);
	CreateWord("sdasda",&w3);
	CreateWord("sdasda",w4);
	CreateWord("sdasda",w5);
	CreateWord("sdasda",w6);
	DestroyWord(&w1);
	DestroyWord(&w2);
	DestroyWord(&w3);
	DestroyWord(w4);
	DestroyWord(w5);
	DestroyWord(w6);
	
	free(w4);
	free(w5);
	free(w6);

	//documents//
	Document d1,d2,d3;
	Document *d4,*d5,*d6;
	d4=malloc(sizeof(Document));
	d5=malloc(sizeof(Document));
	d6=malloc(sizeof(Document));
	int i=0;
	CreateDocument(i++,"sa",&d1);
	CreateDocument(i++,"sa",&d2);
	CreateDocument(i++,"sa",&d3);
	CreateDocument(i++,"sa",d4);
	CreateDocument(i++,"sa",d5);
	CreateDocument(i++,"sa",d6);
	//printf("before destroys\n");
	DestroyDocument(&d1);
	DestroyDocument(&d2);
	DestroyDocument(&d3);
	DestroyDocument(d4);
	DestroyDocument(d5);
	DestroyDocument(d6);
	
	CreateDocument(i++,"sa",&d1);
	CreateDocument(i++,"sa",&d2);
	CreateDocument(i++,"sa",&d3);
	CreateDocument(i++,"sa",d4);
	CreateDocument(i++,"sa",d5);
	CreateDocument(i++,"sa",d6);
	DestroyDocument(&d1);
	DestroyDocument(&d2);
	DestroyDocument(&d3);
	DestroyDocument(d4);
	DestroyDocument(d5);
	DestroyDocument(d6);

	//printf("before frees\n");
	free(d4);
	free(d5);
	free(d6);
	
*/	
/*	//entries//
	Entry e1,e2,e3;
	Entry *e4,*e5,*e6;
	Word w1;
//	CreateWord("asdsa",&w1);	

	e4 =malloc(sizeof(Entry));
	e5 = malloc(sizeof(Entry));
	e6 = malloc(sizeof(Entry));
	CreateWord("asdasda",&w1);
	CreateEntry(&w1,&e1);
	CreateEntry(&w1,&e2);
	CreateEntry(&w1,&e3);
	CreateEntry(&w1,e4);
	CreateEntry(&w1,e5);
	CreateEntry(&w1,e6);
	DestroyEntry(&e1);
	DestroyEntry(&e2);
	DestroyEntry(&e3);
	DestroyEntry(e4);
	DestroyEntry(e5);
	DestroyEntry(e6);

	CreateEntry(&w1,&e1);
	CreateEntry(&w1,&e2);
	CreateEntry(&w1,&e3);
	CreateEntry(&w1,e4);
	CreateEntry(&w1,e5);
	CreateEntry(&w1,e6);
	DestroyEntry(&e1);
	DestroyEntry(&e2);
	DestroyEntry(&e3);
	DestroyEntry(e4);
	DestroyEntry(e5);
	DestroyEntry(e6);
	free(e4);
	free(e5);
	free(e6);
	
	DestroyWord(&w1);
*/	
	//entrylists//
	EntryList el1,el2,el3;
	EntryList *el4,*el5,*el6;
	Entry e1;
	Word w1;

	CreateWord("asdsada",&w1);
	CreateEntry(&w1,&e1);

//	el4=malloc(sizeof(EntryList));
//	el5=malloc(sizeof(EntryList));
//	el6=malloc(sizeof(EntryList));
	CreateEntryList(&el1);
	AddEntry(&el1,&e1);
	DestroyEntry(&e1);
	DestroyEntryList(&el1);
//	CreateEntryList(&el2);
//	CreateEntryList(&el3);
//	CreateEntryList(el4);
//	CreateEntryList(el5);
//	CreateEntryList(el6);
	
//	DestroyEntryList(&el1);
//	DestroyEntryList(&el2);
//	DestroyEntryList(&el3);
//	DestroyEntryList(el4);
//	DestroyEntryList(el5);
//	DestroyEntryList(el6);
	
//	CreateEntryList(&el1);
//	CreateEntryList(&el2);
//	CreateEntryList(&el3);
//	CreateEntryList(el4);
//	CreateEntryList(el5);
//	CreateEntryList(el6);
	
//	DestroyEntryList(&el1);
//	DestroyEntryList(&el2);
//	DestroyEntryList(&el3);
//	DestroyEntryList(el4);
//	DestroyEntryList(el5);
//	DestroyEntryList(el6);
//	free(el4);
//	free(el5);
//	free(el6);

/*	
	//Index
	Index i1,i2,i3;
	Index *i4,*i5,*i6;
	i4=malloc(sizeof(Index));
	i5=malloc(sizeof(Index));
	i6=malloc(sizeof(Index));
	
	EntryList* allpointrEL = malloc(sizeof(EntryList));
	CreateEntryList(allpointrEL);
	Entry en1;
	Word ww1;
	CreateWord("dsdsdsds",&ww1);
		CreateEntry(&ww1,&en1);
		AddEntry(allpointrEL,&en1);
			BuildEntryIndex(allpointrEL,MT_EDIT_DIST,&i1);
			BuildEntryIndex(allpointrEL,MT_EDIT_DIST,&i2);
			BuildEntryIndex(allpointrEL,MT_EDIT_DIST,&i3);
			BuildEntryIndex(allpointrEL,MT_EDIT_DIST,i4);
			BuildEntryIndex(allpointrEL,MT_EDIT_DIST,i5);
			BuildEntryIndex(allpointrEL,MT_EDIT_DIST,i6);
			
			DestroyEntryIndex(&i1);
			DestroyEntryIndex(&i2);
			DestroyEntryIndex(&i3);			
			DestroyEntryIndex(i4);
			DestroyEntryIndex(i5);
			DestroyEntryIndex(i6);
			
			BuildEntryIndex(allpointrEL,MT_HAMMING_DIST,&i1);
			BuildEntryIndex(allpointrEL,MT_HAMMING_DIST,&i2);
			BuildEntryIndex(allpointrEL,MT_HAMMING_DIST,&i3);
			BuildEntryIndex(allpointrEL,MT_HAMMING_DIST,i4);
			BuildEntryIndex(allpointrEL,MT_HAMMING_DIST,i5);
			BuildEntryIndex(allpointrEL,MT_HAMMING_DIST,i6);
	
			DestroyEntryIndex(&i1);
			DestroyEntryIndex(&i2);
			DestroyEntryIndex(&i3);			
			DestroyEntryIndex(i4);
			DestroyEntryIndex(i5);
			DestroyEntryIndex(i6);

			BuildEntryIndex(allpointrEL,MT_EXACT_MATCH,&i1);
			BuildEntryIndex(allpointrEL,MT_EXACT_MATCH,&i2);
			BuildEntryIndex(allpointrEL,MT_EXACT_MATCH,&i3);
			BuildEntryIndex(allpointrEL,MT_EXACT_MATCH,i4);
			BuildEntryIndex(allpointrEL,MT_EXACT_MATCH,i5);
			BuildEntryIndex(allpointrEL,MT_EXACT_MATCH,i6);
	
			DestroyEntryIndex(&i1);
			DestroyEntryIndex(&i2);
			DestroyEntryIndex(&i3);			
			DestroyEntryIndex(i4);
			DestroyEntryIndex(i5);
			DestroyEntryIndex(i6);
		
		DestroyEntry(&en1);
		DestroyEntryList(allpointrEL);
	DestroyWord(&ww1);
	free(i4);
	free(i5);
	free(i6);
	free(allpointrEL);
	
	//if(malloc(512))
	//	printf("Malloc\n");
*/	
	
	
	
}
