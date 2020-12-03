#pragma once

typedef struct _CC_HASH_TABLE CC_HASH_TABLE;

typedef struct _CC_HASH_TABLE_ITERATOR CC_HASH_TABLE_ITERATOR;

int HtCreate(CC_HASH_TABLE **HashTable); // ** Works and tested
int HtDestroy(CC_HASH_TABLE **HashTable); // ** Works and tested

// Returns -1 if Key already exist in HashTable or the parameters are invalid
int HtSetKeyValue(CC_HASH_TABLE *HashTable, char *Key, int Value); // ** Works and tested

// Returns -1 if Key does not exist in HashTable or the parameters are invalid
int HtGetKeyValue(CC_HASH_TABLE *HashTable, char *Key, int *Value); // ** Works and tested

// Returns -1 if Key does not exist in HashTable or the parameters are invalid
int HtRemoveKey(CC_HASH_TABLE *HashTable, char *Key); // ** Works and tested

//  Returns:
//       1  - HashTable contains Key
//       0  - HashTable does not contain Key
//      -1  - Error or invalid parameter
int HtHasKey(CC_HASH_TABLE *HashTable, char *Key); // ** Works and tested

// Initializes the iterator and gets the first key in the hash table
// Returns:
//       -1 - Error or invalid parameter
//       -2 - No keys in the hash table
//      >=0 - Success
int HtGetFirstKey(CC_HASH_TABLE *HashTable, CC_HASH_TABLE_ITERATOR **Iterator, char **Key); // ** Works and tested

// Returns the next key in the hash table contained in the iterator
// Iterator saves the state of the iteration
// Returns:
//       -1 - Error or invalid parameter
//       -2 - No more keys in the hash table
//      >=0 - Success
int HtGetNextKey(CC_HASH_TABLE_ITERATOR *Iterator, char **Key); // ** Works and tested

int HtReleaseIterator(CC_HASH_TABLE_ITERATOR **Iterator); // ** Works and tested

// Removes every element in the hash table
int HtClear(CC_HASH_TABLE *HashTable); // ** Works and tested

// Returns the number of keys in the HashTable, or -1 in case of error
int HtGetKeyCount(CC_HASH_TABLE *HashTable); // ** Works and tested
