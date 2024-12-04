#ifndef DEFN_IMPL_H
#define	DEFN_IMPL_H
#include "primitive_data_struct.h"
#ifdef	__cplusplus
extern "C" {
#endif
	
	struct Word {
		char* characters;
		int length;
		unsigned int hash;
	} ;
	
	struct Query {
		int q_id;
		int n_words;
		enum MatchType type;
		unsigned int dist;
		Word wordz[5];
	};
	
	struct Document {
		Word* words;
		int length;
		int doc_id;
		list_int_stack* Queries;
	};
	
	void PrintDocument(Document doc);
	
#ifdef	__cplusplus
}
#endif

#endif	/* DEFN_IMPL_H */
