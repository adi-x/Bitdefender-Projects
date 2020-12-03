#pragma once

#include "ccvector.h"

typedef struct _CC_HEAP{
    // Members
    CC_VECTOR* Array;
    int isMax;
} CC_HEAP;

// HpCreateMaxHeap and HpCreateMinHeap should create a max heap or a min heap,
// respectively. InitialElements is an optional parameter and, if it is not null, the constructed
// heap should initially contain all the elements in the provided vector.
int HpCreateMaxHeap(CC_HEAP **MaxHeap, CC_VECTOR* InitialElements); // ** Works and tested
int HpCreateMinHeap(CC_HEAP **MinHeap, CC_VECTOR* InitialElements);
int HpDestroy(CC_HEAP **Heap); // ** Works and tested

int HpInsert(CC_HEAP *Heap, int Value); // ** Works and tested

// HpRemove should remove all elements with the value Value in the heap
int HpRemove(CC_HEAP *Heap, int Value); // ** Works and tested

// HpGetExtreme should return the maximum/minimum value in the heap, depending on the
// type of heap constructed
int HpGetExtreme(CC_HEAP *Heap, int* ExtremeValue); // ** Works and tested

// HpPopExtreme should return the maximum/minimum value in the heap, and remove all
// instances of said value from the heap
int HpPopExtreme(CC_HEAP *Heap, int* ExtremeValue); // ** Works and tested

// Returns the number of elements in Heap or -1 in case of error or invalid parameter
int HpGetElementCount(CC_HEAP *Heap); // ** Works and tested

// HpSortToVector should construct and return (in the SortedVector parameter) a vector
// sorted in increasing order containing all the elements present in the heap
int HpSortToVector(CC_HEAP *Heap, CC_VECTOR* SortedVector);
