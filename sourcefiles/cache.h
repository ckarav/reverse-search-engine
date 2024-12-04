#ifndef CACHE_H
#define CACHE_H


#include "defn.h"
#include "index.h"
#include "defn_impl.h"
#include "index_impl.h"

#define CACHE_SIZE 5000
#define MAX_BUCKET_SIZE 10
typedef struct cache_entry{
    Word word;
    EntryList ExResult;
    EntryList EditRes1;
    EntryList EditRes2;
    EntryList EditRes3;
    EntryList HamRes1;
    EntryList HamRes2;
    EntryList HamRes3;
}cache_entry;

typedef struct cache_list_node{
    cache_entry *data;
    struct cache_list_node *next;
    struct cache_list_node *prev;
    struct cache_list_node *bucket_node;//corresponding bucket_node if this is cache_node
    struct cache_list_node *cache_node;//corresponding cache_node if this is bucket_node
}cache_list_node;

typedef struct cache_list{
    cache_list_node *head;
    cache_list_node *tail;
    int list_length;
}cache_list;

typedef struct Cache{
    cache_list CacheMap[CACHE_SIZE];
    cache_list CacheList;
}Cache;

//functions
void init_cache(Cache *c);
void destroy_cache(Cache *c);
cache_list_node* add_at_start(cache_list *cl, cache_entry *ce);
void remove_tail(cache_list *cl);
void add_to_cache(cache_entry *dat, Cache *c);
cache_entry *cache_lookup(Cache *c, Word *w);
////////////
void destroy_cache_entry(cache_entry *ce);
void remove_node(cache_list *cl, cache_list_node *node);
void init_cache_entry(cache_entry *ce, Word *w);

#endif
