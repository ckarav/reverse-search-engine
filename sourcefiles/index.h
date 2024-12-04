#ifndef INDEX_H
#define	INDEX_H

#include "core.h"
#include "defn.h"
#include "primitive_data_struct.h"
//#include "data_struct.h"

#ifdef	__cplusplus
extern "C" {
#endif
	
	
	// === Structures === //
	
	/*
	 * This structure holds a generic information about a particlar entry.
	 */
	typedef struct Payload Payload;
	
	
	struct Payload{
		list_int_stack* queryPos_list;//list of qids//
	};
	/*
	 * This structure holds information about a particlar entry.
	 */
	typedef struct Entry Entry;
	
	struct Entry {
		Word* word;
		Payload* payload;
	};
	
	/*
	 * This structure holds a list of entries.
	 */
	
	
	
	
	
	typedef struct EntryList EntryList;
	
	
	/*
	 * This structure holds the index.
	 */
	typedef struct Index Index;
	typedef struct HammingStruct HammingStruct;
	
	/*
	 * Initialize the given entry with the given word.
	 */
	enum ErrorCode CreateEntry(const Word* word, Entry* entry);
	
	
	/*
	 * Destroy the given entry.
	 */
	enum ErrorCode DestroyEntry(Entry *entry);
	
	
	/*
	 * Initialize the given EntryList.
	 */
	enum ErrorCode CreateEntryList(EntryList* entry_list);
	
	
	/*
	 * Returns the number of entries of the given entryList.
	 */
	unsigned int GetNumberOfEntries(const EntryList* entry_list);
	
	
	/*
	 * Add a new entry at the given entryList.
	 */
	enum ErrorCode AddEntry(EntryList* entry_list, const Entry* entry);
	
	
	/*
	 * Returns the first entry of the given entryList.
	 */
	Entry* GetFirst(EntryList* entry_list);
	
	
	/*
	 * Returns the next entry of the given entryList.
	 */
	Entry* GetNext(EntryList* entry_list);
	
	
	/*
	 * Destroys the given entryList.
	 */
	enum ErrorCode DestroyEntryList(EntryList* entry_list);
	
	
	/*
	 * Create a new index from the contents of a given entryList
	 *
	 * @param[in] entries
	 *   The set of entries in which the index will be built
	 *
	 * @param[in] type
	 *   The matching type of the index as specified in MatchType enumeration
	 *
	 * @param[out] index
	 *   Returns struct in which the index was built
	 */
	enum ErrorCode BuildEntryIndex(EntryList* entry_list,
								   enum MatchType type,
								   Index* index);
	
	enum ErrorCode BuildHammingStruct(EntryList *entry_list,
									  HammingStruct *HamStruct);
	
	/*
	 * Returns the set of words which match with the given word, given a threshold
	 *
	 * @param[in] word
	 *   The word which is searched
	 *
	 * @param[in] threshold
	 *   The acceptable deviation limit of the search
	 *
	 * @param[out] result
	 *   Returns the list of entries which match with the given word
	 */
	enum ErrorCode LookupEntryIndex(const Word* word,
									int threshold,
									const Index* index,
									EntryList* result);
	
	
	/*
	 * Destroys a given index
	 */
	enum ErrorCode DestroyEntryIndex(Index* index);
	enum ErrorCode DestroyHammingStruct(HammingStruct *HamStruct);
	
#ifdef	__cplusplus
}
#endif

#endif	/* INDEX_H */
