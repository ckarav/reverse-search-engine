#include "cache.h"
#include <stdlib.h>
#include <string.h>
void init_cache_entry(cache_entry *ce, Word *w){
    ce->word.characters = malloc((w->length+1)*sizeof(char));
    strcpy(ce->word.characters,w->characters);
    ce->word.length = w->length;
    ce->word.hash = w->hash;
    ////////
    CreateEntryList(&(ce->ExResult));
    CreateEntryList(&(ce->EditRes1));
    CreateEntryList(&(ce->EditRes2));
    CreateEntryList(&(ce->EditRes3));
    CreateEntryList(&(ce->HamRes1));
    CreateEntryList(&(ce->HamRes2));
    CreateEntryList(&(ce->HamRes3));
}


void destroy_cache_entry(cache_entry *ce){
    DestroyWord(&(ce->word));
    DestroyEntryList(&(ce->ExResult));
    DestroyEntryList(&(ce->EditRes1));
    DestroyEntryList(&(ce->EditRes2));
    DestroyEntryList(&(ce->EditRes3));
    DestroyEntryList(&(ce->HamRes1));
    DestroyEntryList(&(ce->HamRes2));
    DestroyEntryList(&(ce->HamRes3));
}

void remove_node(cache_list *cl, cache_list_node *node){
    if(cl->list_length == 1){
        free(node);//remove the one node
        cl->head = NULL;
        cl->tail = NULL;
        cl->list_length = 0;
    }
    else{//case of more than one elements in this bucket list
        if(node == cl->head){//remove head
            cl->head = node->next;
            free(node);
            cl->head->prev = NULL;
        }
        else if(node == cl->tail){//remove tail
            cl->tail = node->prev;
            free(node);
            cl->tail->next = NULL;
        }
        else{//remove intermediate node
            node->prev->next = node->next;
            node->next->prev = node->prev;
            free(node);
        }
        cl->list_length--;
    }
}

void init_cache(Cache *c){
    int i;
    for(i=0;i<CACHE_SIZE;i++){
        c->CacheMap[i].head = NULL;
        c->CacheMap[i].tail = NULL;
        c->CacheMap[i].list_length = 0;
    }
    c->CacheList.head = NULL;
    c->CacheList.tail = NULL;
    c->CacheList.list_length = 0;
}

cache_list_node* add_at_start(cache_list *cl, cache_entry *ce){
    cache_list_node *temp;
    temp = malloc(sizeof(cache_list_node));
    temp->data = ce;
    temp->prev = NULL;
    temp->next = cl->head;
    cl->head = temp;
    if(cl->list_length == 0)
        cl->tail = temp;
    else
        temp->next->prev = temp;
    cl->list_length++;
    return temp;
}

void remove_tail(cache_list *cl){
    destroy_cache_entry(cl->tail->data);//destroy data
    free(cl->tail->data);
    cl->tail = cl->tail->prev;
    free(cl->tail->next);
    cl->tail->next = NULL;
    cl->list_length--;
}

void add_to_cache(cache_entry *dat, Cache *c){
    cache_list_node *bucket_nodez, *cache_nodez;
    int i;
    //////////////
	if(c->CacheList.list_length<CACHE_SIZE){//cache not full yet
        int pos = dat->word.hash%(CACHE_SIZE);
        ///check if bucket is full
        if(c->CacheMap[pos].list_length==MAX_BUCKET_SIZE){
            destroy_cache_entry(dat);
            free(dat);
            return;
        }
        /////////////
        bucket_nodez = add_at_start(&(c->CacheMap[pos]),dat);
        cache_nodez = add_at_start(&(c->CacheList),dat);
        //////////////
        bucket_nodez->bucket_node = NULL;
        bucket_nodez->cache_node = cache_nodez;
        /////////////
        cache_nodez->bucket_node = bucket_nodez;
        cache_nodez->cache_node = NULL;
        //////////////
    }
    else{
        ///check if bucket is full
        int pos = dat->word.hash%(CACHE_SIZE);
        if(c->CacheMap[pos].list_length==MAX_BUCKET_SIZE){
            destroy_cache_entry(dat);
            free(dat);
            return;
        }
        /////////////
        //must remove least recently used cache_entry
        cache_list_node *lru_entry = c->CacheList.tail;
        cache_list_node *buck_node = lru_entry->bucket_node;
        pos = lru_entry->data->word.hash%(CACHE_SIZE);
        remove_tail(&(c->CacheList));//remove lru entry from cache
        remove_node(&(c->CacheMap[pos]),buck_node);//remove corresponding bucket node
        ///////////////////////
        //add new entry
        pos = dat->word.hash%(CACHE_SIZE);
        bucket_nodez = add_at_start(&(c->CacheMap[pos]),dat);
        cache_nodez = add_at_start(&(c->CacheList),dat);
        ////////////
        bucket_nodez->bucket_node = NULL;
        bucket_nodez->cache_node = cache_nodez;
        ////////////
        cache_nodez->bucket_node = bucket_nodez;
        cache_nodez->cache_node = NULL;
    }
}


cache_entry *cache_lookup(Cache *c, Word *w){
    cache_list *bucket;
    cache_list_node *temp;
    bucket = &(c->CacheMap[w->hash%(CACHE_SIZE)]);
    temp = bucket->head;
    if(temp == NULL)//empty bucket
        return NULL;
    while(temp!=NULL){
        if(!strcmp(temp->data->word.characters,w->characters))
            break;
        temp = temp->next;
    }
    if(temp == NULL)//word is not in cache
        return NULL;
    else{//word was found
        //updating the cache entry by placing it at the head of the cache list
        temp = temp->cache_node;//set temp to point to the corresponding cache list node
        if(temp == c->CacheList.head)
            return temp->data;
        else{//case of >1 elements and element not at head of list
            if(temp == c->CacheList.tail){
                temp->prev->next = NULL;
                c->CacheList.tail = temp->prev;
                temp->prev = NULL;
                temp->next = c->CacheList.head;
                c->CacheList.head->prev = temp;
                c->CacheList.head = temp;
            }
            else{//intermediate element
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                temp->prev = NULL;
                temp->next = c->CacheList.head;
                c->CacheList.head->prev = temp;
                c->CacheList.head = temp;
            }
        }
        return temp->data;
    }
}

void destroy_cache(Cache *c){
    cache_list_node *temp, *temp1;
    int i;
    //free cache map
    for(i=0;i<CACHE_SIZE;i++){
        temp = c->CacheMap[i].head;
        while(temp!=NULL){
            destroy_cache_entry(temp->data);
            free(temp->data);
            temp1 = temp->next;
            free(temp);
            temp = temp1;
        }
    }
    //free cache list
    temp = c->CacheList.head;
    while(temp!=NULL){
        temp1 = temp->next;
        free(temp);
        temp = temp1;
    }
}





