
#ifndef HASH_H
#define HASH_H

#include "defn.h"
#include "defn_impl.h"
int M_HashFunction(Word key,int doc_size); //interface of simplest muliplier function for strings, returns modulo docsize//
unsigned int M_HashCode(Word key);//mult hashfunction for word keys, it is used by M_HashFunction//
unsigned int * MD5_HashFunction(Word key,unsigned int* digest);//inteface of MD5 hash function//
#endif