/*********************************************/
/*custom implementation of sorting algorithms*/
/*********************************************/
#ifndef DATA_SORT
#define DATA_SORT
#include "primitive_data_struct.h"
unsigned GetDigit(int num,int index);//retuns the decimal digit at position 'index' LSD style ex GetDigit(1234,0) = 4//
int GetDigitsNum(int num);//returns the number of decimal digits that the num is represented by//
int GetMax(int* array,int length);//retunrs tha maximum integer from tha array 'array' with size 'length'//
int RadixSort(int* array,int length);//returns the median (low rounding) of an int array//
list_int_stack* RadixSortList(list_int_stack* original);//sorts the original list and returns it's sorted alias address//

#endif