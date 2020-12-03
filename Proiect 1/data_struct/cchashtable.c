#include "cchashtable.h"
#include "common.h"
#include "stdint.h"
#include "string.h"

#define INITIAL_SIZE 11
#define RESIZE_FACTOR(N) ((N) * 2 + 1)

typedef struct ListItem {
    int Value;
    char* Key;
    struct ListItem* Next;
} ListItem;

typedef struct _CC_HASH_TABLE {
    // Members
    ListItem** Array;
    int Size;
    int Count;
} CC_HASH_TABLE;

typedef struct _CC_HASH_TABLE_ITERATOR
{
    CC_HASH_TABLE* HashTable; // set by call to HtGetFirstKey
    ListItem* LastItem;
    int Index;
    // Other Member
} CC_HASH_TABLE_ITERATOR;

int StringLength(char* String)
{
    if (String == NULL)
    {
        return -1;
    }
    int Len = 0;
    while (String[Len] != '\0')
    {
        Len++;
    }
    return Len;
}

int StringCopy(char* Dest, char* Source)
{
    if (Dest == NULL)
    {
        return -1;
    }
    if (Source == NULL || Source[0] == '\0')
    {
        return -1;
    }
    int i;
    for (i = 0; Source[i] != '\0'; i++)
    {
        Dest[i] = Source[i];
    }
    Dest[i] = '\0';
    return 0;
}

int StringCompare(char* String1, char* String2)
{
    if (String1 == NULL)
    {
        return -1;
    }
    if (String2 == NULL)
    {
        return -1;
    }
    int i = 0;
    while (String1[i] == String2[i])
    {
        if (String1[i] == '\0')
            return 1;
        i++;
    }
    return 0;
}

uint32_t HashFunction(const void* Key, int len)
{
    uint32_t seed = 0xe216b920;
    uint32_t m = 0x5bd1e995;
    int r = 24;
 
    uint32_t h = seed ^ len;

    const unsigned char* data = (const unsigned char*) Key;

    while (len >= 4)
    {
        uint32_t k = *(uint32_t*)data;
        k ^= k >> r;
        k *= m;
        
        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0];
        h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

int Rehash(CC_HASH_TABLE* HashTable)
{
    if (HashTable == NULL)
    {
        return -1;
    }
    int newSize = RESIZE_FACTOR(HashTable->Size);
    ListItem** array = NULL;
    array = malloc(sizeof(ListItem*) * newSize);
    if (array == NULL)
    {
        return -1;
    }
    memset(array, 0, newSize * sizeof(ListItem*));
    ListItem** tempStorage = NULL;
    ListItem* it = NULL;
    ListItem* itAux = NULL;
    int ret = -1;
    int oldSize = HashTable->Size;
    tempStorage = HashTable->Array;
    HashTable->Array = array;
    HashTable->Count = 0;
    HashTable->Size = newSize;
    for (int i = 0; i < oldSize; i++)
    {
        if (tempStorage[i] != NULL)
        {
            it = tempStorage[i];
            while (it->Next != NULL)
            {
                ret = HtSetKeyValue(HashTable, it->Key, it->Value);
                if (ret == -1)
                {
                    return -1;
                }
                free(it->Key);
                itAux = it->Next;
                free(it);
                it = itAux;
            }
            ret = HtSetKeyValue(HashTable, it->Key, it->Value);
            if (ret == -1)
            {
                return -1;
            }
            free(it->Key);
            free(it);
        }
    }
    free(tempStorage);
    return 0;
}

int HtCreate(CC_HASH_TABLE **HashTable)
{
    CC_UNREFERENCED_PARAMETER(HashTable);

    if (HashTable == NULL)
    {
        return -1;
    }

    CC_HASH_TABLE* hashTable = NULL;
    hashTable = malloc(sizeof(CC_HASH_TABLE));

    if (hashTable == NULL)
    {
        return -1;
    }

    ListItem** array = NULL;
    array = malloc(sizeof(ListItem*) * INITIAL_SIZE);
    if (array == NULL)
    {
        free(hashTable);
        return -1;
    }
    memset(array, 0, INITIAL_SIZE * sizeof(ListItem*));
    hashTable->Array = array;
    hashTable->Count = 0;
    hashTable->Size = INITIAL_SIZE;

    *HashTable = hashTable;

    return 0;
}

int HtDestroy(CC_HASH_TABLE **HashTable)
{
    CC_UNREFERENCED_PARAMETER(HashTable);

    if (HashTable == NULL)
    {
        return -1;
    }

    CC_HASH_TABLE* hashTable = *HashTable;

    for (int i = 0; i < hashTable->Size; i++)
    {
        
        if (hashTable->Array[i] != NULL)
        {
            ListItem* actualItem = hashTable->Array[i];
            ListItem* aux = NULL;
            while (actualItem->Next != NULL)
            {
                aux = actualItem->Next;
                free(actualItem->Key);
                free(actualItem);
                actualItem = aux;
            }
            if (actualItem != NULL)
            {
                free(actualItem);
            }
        }
    }
    free(hashTable->Array);
    free(hashTable);
    *HashTable = NULL;

    return 0;
}

int HtSetKeyValue(CC_HASH_TABLE *HashTable, char *Key, int Value)
{
    CC_UNREFERENCED_PARAMETER(HashTable);
    CC_UNREFERENCED_PARAMETER(Key);
    CC_UNREFERENCED_PARAMETER(Value);
    if (HashTable == NULL)
    {
        return -1;
    }
    if (Key == NULL)
    {
        return -1;
    }
    if (Key[0] == '\0')
    {
        return -1;
    }
    if (HtHasKey(HashTable, Key) == 1)
    {
        return -1;
    }
    int len = 0;
    for (; Key[len] != '\0'; len++);
    unsigned hash = HashFunction(Key, len);
    int index = hash % HashTable->Size;

    if (HashTable->Array[index] == NULL)
    {

        ListItem* newList = malloc(sizeof(ListItem));
        if (newList == NULL)
        {
            return -1;
        }
        char* keyClone = NULL;
        keyClone = malloc((StringLength(Key) + 1) * sizeof(char));
        StringCopy(keyClone, Key);
        newList->Key = keyClone;
        newList->Value = Value;
        newList->Next = NULL;
        HashTable->Array[index] = newList;
        HashTable->Count++;
        if ((HashTable->Count * 1.0) / HashTable->Size > 0.75)
        {
            Rehash(HashTable);
        }
    }
    else
    {
        ListItem* it;
        it = HashTable->Array[index];
        while (it->Next != NULL)
        {
            if (StringCompare(it->Key, Key) == 1)
            {
                it->Value = Value;
                return 0;
            }
            it = it->Next;
        }
        if (StringCompare(it->Key, Key) == 1)
        {
            it->Value = Value;
            return 0;
        }
        ListItem* newList = malloc(sizeof(ListItem));
        if (newList == NULL)
        {
            return -1;
        }
        char* keyClone = NULL;
        keyClone = malloc((StringLength(Key) + 1) * sizeof(char));
        StringCopy(keyClone, Key);
        newList->Key = keyClone;
        newList->Value = Value;
        newList->Next = NULL;
        it->Next = newList;
        HashTable->Count++;
        if ((HashTable->Count * 1.0) / HashTable->Size > 0.75)
        {
            Rehash(HashTable);
        }
    }
    return 0;

}

int HtGetKeyValue(CC_HASH_TABLE *HashTable, char *Key, int *Value)
{
    CC_UNREFERENCED_PARAMETER(HashTable);
    CC_UNREFERENCED_PARAMETER(Key);
    CC_UNREFERENCED_PARAMETER(Value);
    if (HashTable == NULL)
    {
        return -1;
    }
    if (Key == NULL)
    {
        return -1;
    }
    if (Key[0] == '\0')
    {
        return -1;
    }
    if (Value == NULL)
    {
        return -1;
    }
    int len = 0;
    for (; Key[len] != '\0'; len++);
    unsigned hash = HashFunction(Key, len);
    int index = hash % HashTable->Size;
    if (HashTable->Array[index] == NULL)
    {
        return -1;
    }
    ListItem* it = HashTable->Array[index];
    while (StringCompare(it->Key, Key) == 0)
    {
        it = it->Next;
        if (it == NULL)
        {
            return -1;
        }
    }
    *Value = it->Value;
    return 0;
}

int HtRemoveKey(CC_HASH_TABLE *HashTable, char *Key)
{
    CC_UNREFERENCED_PARAMETER(HashTable);
    CC_UNREFERENCED_PARAMETER(Key);
    if (HashTable == NULL)
    {
        return -1;
    }
    if (Key == NULL)
    {
        return -1;
    }
    if (Key[0] == '\0')
    {
        return -1;
    }
    int len = 0;
    for (; Key[len] != '\0'; len++);
    unsigned hash = HashFunction(Key, len);
    int index = hash % HashTable->Size;
    if (HashTable->Array[index] == NULL)
    {
        return -1;
    }
    ListItem* itPrev = NULL;
    ListItem* it = HashTable->Array[index];
    while (StringCompare(it->Key, Key) == 0)
    {
        itPrev = it;
        it = it->Next;
        if (it == NULL)
        {
            return -1;
        }
    }
    if (itPrev == NULL)
    {
        HashTable->Array[index] = it->Next;
    }
    else 
    {
        itPrev->Next = it->Next;
    }
    free(it->Key);
    free(it);
    HashTable->Count--;
    return 0;
}

int HtHasKey(CC_HASH_TABLE *HashTable, char *Key)
{
    CC_UNREFERENCED_PARAMETER(HashTable);
    CC_UNREFERENCED_PARAMETER(Key);
    if (HashTable == NULL)
    {
        return -1;
    }
    if (Key == NULL)
    {
        return -1;
    }
    if (Key[0] == '\0')
    {
        return -1;
    }
    int len = 0;
    for (; Key[len] != '\0'; len++);
    unsigned hash = HashFunction(Key, len);
    int index = hash % HashTable->Size;
    if (HashTable->Array[index] == NULL)
    {
        return 0;
    }
    ListItem* itPrev = NULL;
    ListItem* it = HashTable->Array[index];
    while (StringCompare(it->Key, Key) == 0)
    {
        itPrev = it;
        it = it->Next;
        if (it == NULL)
        {
            return 0;
        }
    }
    return 1;
}

int HtGetFirstKey(CC_HASH_TABLE* HashTable, CC_HASH_TABLE_ITERATOR **Iterator, char **Key)
{
    CC_HASH_TABLE_ITERATOR *iterator = NULL;

    CC_UNREFERENCED_PARAMETER(Key);

    if (NULL == HashTable)
    {
        return -1;
    }
    if (NULL == Iterator)
    {
        return -1;
    }
    if (NULL == Key)
    {
        return -1;
    }
    if (Key[0] == '\0')
    {
        return -1;
    }
    if (HashTable->Count == 0)
    {
        return -2;
    }
    iterator = (CC_HASH_TABLE_ITERATOR*)malloc(sizeof(CC_HASH_TABLE_ITERATOR));
    if (NULL == iterator)
    {
        return -1;
    }

    memset(iterator, 0, sizeof(*iterator));

    iterator->HashTable = HashTable;
    iterator->LastItem = NULL;
    iterator->Index = 0;
    *Iterator = iterator;

    for (int i = 0; i < HashTable->Size; i++)
    {
        if (HashTable->Array[i] != NULL)
        {
            char* keyClone = NULL;
            keyClone = malloc(sizeof(char) * (StringLength(HashTable->Array[i]->Key) + 1));
            StringCopy(keyClone, HashTable->Array[i]->Key);
            *Key = keyClone;
            iterator->LastItem = HashTable->Array[i];
            iterator->Index = i;
            break;
        }
    }
    // FIND THE FIRST KEY AND SET Key

    return 0;
}

int HtGetNextKey(CC_HASH_TABLE_ITERATOR *Iterator, char **Key)
{
    CC_UNREFERENCED_PARAMETER(Key);
    CC_UNREFERENCED_PARAMETER(Iterator);

    if (Iterator == NULL)
    {
        return -1;
    }
    if (Key == NULL)
    {
        return -1;
    }
    if (Iterator->Index >= Iterator->HashTable->Size)
    {
        return -1;
    }
    if (Iterator->LastItem->Next != NULL)
    {
        char* nextKey = Iterator->LastItem->Next->Key;
        char* keyClone = NULL;
        keyClone = malloc(sizeof(char) * (StringLength(nextKey) + 1));
        StringCopy(keyClone, nextKey);
        *Key = keyClone;
        return 0;
    }

    for (int i = Iterator->Index + 1; i < Iterator->HashTable->Size; i++)
    {
        if (Iterator->HashTable->Array[i] != NULL)
        {
            char* keyClone = NULL;
            keyClone = malloc(sizeof(char) * (StringLength(Iterator->HashTable->Array[i]->Key) + 1));
            StringCopy(keyClone, Iterator->HashTable->Array[i]->Key);
            *Key = keyClone;
            Iterator->LastItem = Iterator->HashTable->Array[i];
            Iterator->Index = i;
            return 0;
        }
    }
    return -2;
}

int HtReleaseIterator(CC_HASH_TABLE_ITERATOR **Iterator)
{
    CC_UNREFERENCED_PARAMETER(Iterator);
    if (Iterator == NULL)
    {
        return -1;
    }
    CC_HASH_TABLE_ITERATOR* iterator = *Iterator;
    free(iterator);
    *Iterator = NULL;
    return 0;
}

int HtClear(CC_HASH_TABLE *HashTable)
{
    CC_UNREFERENCED_PARAMETER(HashTable);
    if (HashTable == NULL)
    {
        return -1;
    }

    for (int i = 0; i < HashTable->Size; i++) // Freeing the lists.
    {
        if (HashTable->Array[i] != NULL)
        {
            ListItem* actualItem = HashTable->Array[i];
            ListItem* aux = NULL;
            while (actualItem->Next != NULL)
            {
                aux = actualItem->Next;
                free(actualItem->Key);
                free(actualItem);
                actualItem = aux;
            }
            free(actualItem);
        }
    }
    ListItem** temp = realloc(HashTable->Array, sizeof(ListItem*) * INITIAL_SIZE);
    if (temp == NULL)
    {  
        return -1;
    }
    HashTable->Array = temp;
    memset(HashTable->Array, 0, INITIAL_SIZE * sizeof(ListItem*));
    HashTable->Count = 0;
    HashTable->Size = INITIAL_SIZE;
    return 0;
}

int HtGetKeyCount(CC_HASH_TABLE *HashTable)
{
    CC_UNREFERENCED_PARAMETER(HashTable);
    if (HashTable == NULL)
    {
        return -1;
    }
    return HashTable->Count;
}
