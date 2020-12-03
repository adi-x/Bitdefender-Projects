#include "ccvector.h"
#include "common.h"
#include "string.h"

#define INITIAL_SIZE    100
#define GROWTH_FUNCTION(x) ((x) + 100)

int VecCreate(CC_VECTOR **Vector) // Works as expected
{
    CC_VECTOR *vec = NULL;

    CC_UNREFERENCED_PARAMETER(Vector);

    if (NULL == Vector)
    {
        return -1;
    }

    vec = (CC_VECTOR*)malloc(sizeof(CC_VECTOR));
    if (NULL == vec)
    {
        return -1;
    }

    memset(vec, 0, sizeof(*vec));

    vec->Count = 0;
    vec->Size = INITIAL_SIZE;
    vec->Array = (int*)malloc( sizeof(int) * INITIAL_SIZE );
    if (NULL == vec->Array) 
    {
        free(vec);
        return -1;
    }

    *Vector = vec;

    return 0;
}

int VecDestroy(CC_VECTOR **Vector) // Works as expected
{
    CC_VECTOR *vec = *Vector;

    if (NULL == Vector)
    {
        return -1;
    }

    free(vec->Array);
    free(vec);

    *Vector = NULL;

    return 0;
}

int VecInsertTail(CC_VECTOR *Vector, int Value) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    CC_UNREFERENCED_PARAMETER(Value);

    if (NULL == Vector)
    {
        return -1;
    }

    if (Vector->Count >= Vector->Size)
    {
        int NewSize = GROWTH_FUNCTION(Vector->Size);
        int* Temp = realloc(Vector->Array, NewSize * sizeof(int));
        if (Temp == NULL)
        {
            return -1;
        }
        Vector->Size = NewSize;
        Vector->Array = Temp;
    }

    Vector->Array[Vector->Count] = Value;
    Vector->Count++;

    return 0;
}

int VecInsertHead(CC_VECTOR *Vector, int Value) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    CC_UNREFERENCED_PARAMETER(Value);

    if (NULL == Vector)
    {
        return -1;
    }

    if (Vector->Count >= Vector->Size)
    {
        int NewSize = GROWTH_FUNCTION(Vector->Size);
        int* Temp = realloc(Vector->Array, NewSize * sizeof(int));
        if (Temp == NULL)
        {
            return -1;
        }
        Vector->Size = NewSize;
        Vector->Array = Temp;
    }

    for (int i = Vector->Count-1; i >= 0; i--)
    {
        Vector->Array[i + 1] = Vector->Array[i];
    }
    Vector->Array[0] = Value;
    Vector->Count++;

    return 0;
}

int VecInsertAfterIndex(CC_VECTOR *Vector, int Index, int Value) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    CC_UNREFERENCED_PARAMETER(Index);
    CC_UNREFERENCED_PARAMETER(Value);

    if (NULL == Vector)
    {
        return -1;
    }

    if (Vector->Count >= Vector->Size)
    {
        int NewSize = GROWTH_FUNCTION(Vector->Size);
        int* Temp = realloc(Vector->Array, NewSize * sizeof(int));
        if (Temp == NULL)
        {
            return -1;
        }
        Vector->Size = NewSize;
        Vector->Array = Temp;
    }

    if (Index >= Vector->Count)
    {
        return -1;
    }

    for (int i = Vector->Count - 1; i >= Index + 1; i--)
    {
        Vector->Array[i + 1] = Vector->Array[i];
    }
    Vector->Array[Index + 1] = Value;
    Vector->Count++;

    return 0;
}

int VecRemoveByIndex(CC_VECTOR *Vector, int Index) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    CC_UNREFERENCED_PARAMETER(Index);

    if (NULL == Vector)
    {
        return -1;
    }

    if (Vector->Count <= 0)
    {
        return -1; // nothing to remove
    }

    if (Index >= Vector->Count)
    {
        return -1;
    }

    for (int i = Index; i < Vector->Count - 1; i++)
    {
        Vector->Array[i] = Vector->Array[i + 1];
    }

    Vector->Count--;

    return 0;
}

int VecGetValueByIndex(CC_VECTOR *Vector, int Index, int *Value) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    CC_UNREFERENCED_PARAMETER(Index);
    CC_UNREFERENCED_PARAMETER(Value);

    if (NULL == Vector)
    {
        return -1;
    }

    if (Vector->Count <= 0)
    {
        return -1; // vector is empty
    }

    if (Index >= Vector->Count)
    {
        return -1;
    }
    
    *Value = Vector->Array[Index];

    return 0;
}

int VecGetCount(CC_VECTOR *Vector) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    if (NULL == Vector)
    {
        return -1;
    }

    return Vector->Count;
}

int VecClear(CC_VECTOR *Vector) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);

    if (NULL == Vector)
    {
        return -1;
    }

    int NewSize = INITIAL_SIZE;
    int* Temp = realloc(Vector->Array, NewSize * sizeof(int));
    if (Temp == NULL)
    {
        return -1;
    }
    Vector->Size = NewSize;
    Vector->Array = Temp;
    Vector->Count = 0;
    return 0;
}

int Merge(int *Array, int Left, int Middle, int Right)
{
    CC_UNREFERENCED_PARAMETER(Array);
    CC_UNREFERENCED_PARAMETER(Left);
    CC_UNREFERENCED_PARAMETER(Right);
    CC_UNREFERENCED_PARAMETER(Middle);

    int *leftSubarray = NULL;
    int *rightSubarray = NULL;

    int leftSubarraySize = Middle - Left + 1; // LeftArray = Array[Left .. Middle]
    int rightSubarraySize = Right - Middle; // RightArray = Array[Middle + 1 .. Right]
	leftSubarray = malloc(leftSubarraySize * sizeof(int));
	rightSubarray = malloc(rightSubarraySize * sizeof(int));
    if (leftSubarray == NULL || rightSubarray == NULL)
    {
        return -1;
    }
    
    for (int i = 0; i < leftSubarraySize; i++)
    {
        leftSubarray[i] = Array[Left + i];
    }
    
    for (int i = 0; i < rightSubarraySize; i++)
    {
        rightSubarray[i] = Array[Middle + 1 + i];
    }

    
    int arrayIndex = Left;
    int leftSubarrayIndex = 0;
    int rightSubarrayIndex = 0;

    while (leftSubarrayIndex < leftSubarraySize && rightSubarrayIndex < rightSubarraySize)
    {
        if (leftSubarray[leftSubarrayIndex] > rightSubarray[rightSubarrayIndex])
        {
            Array[arrayIndex] = leftSubarray[leftSubarrayIndex];
            leftSubarrayIndex++;
        }
        else
        {
            Array[arrayIndex] = rightSubarray[rightSubarrayIndex];
            rightSubarrayIndex++;
        }
        arrayIndex++;
    }
    
    while (leftSubarrayIndex < leftSubarraySize)
    {
        Array[arrayIndex] = leftSubarray[leftSubarrayIndex];
        leftSubarrayIndex++;
        arrayIndex++;
    }
    
    while (rightSubarrayIndex < rightSubarraySize)
    {
        Array[arrayIndex] = rightSubarray[rightSubarrayIndex];
        rightSubarrayIndex++;
        arrayIndex++;
    }
    
    free(rightSubarray);
    free(leftSubarray);
    
    return 0;
}

int Sort(int* Array, int Left, int Right)
{
    if (Left < Right)
    {
        int Middle = (Left + Right) / 2;
        int res;
        res = Sort(Array, Left, Middle);
        if (res == -1)
        {
            return -1;
        }
        res = Sort(Array, Middle + 1, Right);
        if (res == -1)
        {
            return -1;
        }
        res = Merge(Array, Left, Middle, Right);
        if (res == -1)
        {
            return -1;
        }
    }
    return 0;
}

// We'll be using merge sort for this.
int VecSort(CC_VECTOR *Vector) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(Vector);
    int res = Sort(Vector->Array, 0, Vector->Count - 1);
    if (res == -1)
    {
        return -1;
    }
    return 0;
}

int VecAppend(CC_VECTOR *DestVector, CC_VECTOR *SrcVector) // Works as expected
{
    CC_UNREFERENCED_PARAMETER(DestVector);
    CC_UNREFERENCED_PARAMETER(SrcVector);
    if (DestVector->Count + SrcVector->Count - 1 >= SrcVector->Size)
    {
        int NewSize = GROWTH_FUNCTION(SrcVector->Size + DestVector->Size);
        int* Temp = realloc(SrcVector->Array, NewSize * sizeof(int));
        if (Temp == NULL)
        {
            return -1;
        }
        SrcVector->Size = NewSize;
        SrcVector->Array = Temp;
        return -1;
    }
    int SrcIndex = SrcVector->Count;
    for (int DestIndex = 0; DestIndex < DestVector->Count; DestIndex++, SrcIndex++)
    {
        SrcVector->Array[SrcIndex] = DestVector->Array[DestIndex];
    }
    SrcVector->Count += DestVector->Count;
    return 0;
}
