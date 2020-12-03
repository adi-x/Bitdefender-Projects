#include "ccheap.h"
#include "common.h"


int GetLeftChild(CC_HEAP *Heap, int Index)
{
    if (Heap == NULL)
    {
        return -1;
    }
    if (VecGetCount(Heap->Array) <= (Index * 2) + 1)
    {
        return -1;
    }
    return (Index * 2) + 1;
}
int GetRightChild(CC_HEAP* Heap, int Index)
{
    if (Heap == NULL)
    {
        return -1;
    }
    if (VecGetCount(Heap->Array) <= (Index * 2) + 2)
    {
        return -1;
    }
    return (Index * 2) + 2;
}

int GetParent(int Index)
{
    if (Index == 0)
    {
        return -1;
    }
    return (Index - 1) / 2;
}

int Swap(CC_HEAP* Heap, int Index1, int Index2)
{
    if (Heap == NULL)
    {
        return -1;
    }
    int value1 = 0, value2 = 0;
    int ret = 0;
    ret = VecGetValueByIndex(Heap->Array, Index1, &value1);
    if (ret == -1)
    {
        return -1;
    }
    ret = VecGetValueByIndex(Heap->Array, Index2, &value2);
    if (ret == -1)
    {
        return -1;
    }
    ret = VecRemoveByIndex(Heap->Array, Index1);
    if (ret == -1)
    {
        return -1;
    }
    ret = VecInsertAfterIndex(Heap->Array, Index1 - 1, value2);
    if (ret == -1)
    {
        return -1;
    }
    ret = VecRemoveByIndex(Heap->Array, Index2);
    if (ret == -1)
    {
        return -1;
    }
    ret = VecInsertAfterIndex(Heap->Array, Index2 - 1, value1);
    if (ret == -1)
    {
        return -1;
    }
    return 0;
}

int HeapifyUp(CC_HEAP* Heap, int Index)
{
    int parent;
    int valueParent;
    int value;
    int ret;
    ret = VecGetValueByIndex(Heap->Array, Index, &value);
    if (ret == -1)
    {
        return -1;
    }
    parent = GetParent(Index);
    while (parent != -1)
    {
        ret = VecGetValueByIndex(Heap->Array, parent, &valueParent);
        if (ret == -1)
        {
            return -1;
        }
        
        if (Heap->isMax == 1)
        {
            if (valueParent < value)
            {
                Swap(Heap, parent, Index);
                Index = parent;
                parent = GetParent(Index);
            }
            else
            {
                break;
            }
        }
        else
        {
            if (valueParent > value)
            {
                Swap(Heap, parent, Index);
                Index = parent;
                parent = GetParent(Index);
            }
            else
            {
                break;
            }
        }
    }
    return 0;
}

int HpCreateMaxHeap(CC_HEAP **MaxHeap, CC_VECTOR* InitialElements)
{
    CC_UNREFERENCED_PARAMETER(MaxHeap);
    CC_UNREFERENCED_PARAMETER(InitialElements);
    if (MaxHeap == NULL)
    {
        return -1;
    }
    CC_HEAP* Heap = NULL;
    Heap = malloc(sizeof(CC_HEAP));
    if (Heap == NULL)
    {
        return -1;
    }

    Heap->isMax = 1;
    CC_VECTOR* Array = NULL;
    int ret;
    ret = VecCreate(&Array);
    if (ret == -1)
    {
        return -1;
    }
    Heap->Array = Array;
    *MaxHeap = Heap;
    if (InitialElements != NULL)
    {
        int value;
        int count = VecGetCount(InitialElements);
        for (int i = 0; i < count; i++)
        {
            ret = VecGetValueByIndex(InitialElements, i, &value);
            if (ret == -1)
            {
                return -1;
            }
            ret = HpInsert(Heap, value);
            if (ret == -1)
            {
                return -1;
            }
        }
    }
    return 0;
}


int HpCreateMinHeap(CC_HEAP **MinHeap, CC_VECTOR* InitialElements)
{
    CC_UNREFERENCED_PARAMETER(MinHeap);
    CC_UNREFERENCED_PARAMETER(InitialElements);
    if (MinHeap == NULL)
    {
        return -1;
    }
    CC_HEAP* Heap = NULL;
    Heap = malloc(sizeof(CC_HEAP));
    if (Heap == NULL)
    {
        return -1;
    }

    Heap->isMax = 0;
    CC_VECTOR* Array = NULL;
    int ret;
    ret = VecCreate(&Array);
    if (ret == -1)
    {
        return -1;
    }
    Heap->Array = Array;
    *MinHeap = Heap;
    if (InitialElements != NULL)
    {
        int value;
        int count = VecGetCount(InitialElements);
        for (int i = 0; i < count; i++)
        {
            ret = VecGetValueByIndex(InitialElements, i, &value);
            if (ret == -1)
            {
                return -1;
            }
            ret = HpInsert(Heap, value);
            if (ret == -1)
            {
                return -1;
            }
        }
    }
    return 0;
}


int HpDestroy(CC_HEAP **Heap)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    if (Heap == NULL)
    {
        return -1;
    }
    int ret;
    CC_HEAP* heap = *Heap;
    ret = VecDestroy(&heap->Array);
    if (ret == -1)
    {
        return -1;
    }
    free(heap);
    *Heap = NULL;

    return 0;
}

int HpInsert(CC_HEAP *Heap, int Value)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Heap == NULL)
    {
        return -1;
    }
    int index = VecGetCount(Heap->Array);
    int ret;
    if (index == -1)
    {
        return -1;
    }
    ret = VecInsertTail(Heap->Array, Value);
    if (ret == -1)
    {
        return -1;
    }
    ret = HeapifyUp(Heap, index);
    if (ret == -1)
    {
        return -1;
    }

    return 0;
}

int HeapifyDown2(CC_HEAP* Heap, int Index)
{
    int left = GetLeftChild(Heap, Index);
    int right = GetRightChild(Heap, Index);
    int better = Index;
    int leftValue = 0;
    int rightValue = 0;
    int value = 0;
    int ret = 0;
    ret = VecGetValueByIndex(Heap->Array, left, &leftValue);
    if (ret == -1)
    {
        return -1;
    }
    ret = VecGetValueByIndex(Heap->Array, right, &rightValue);
    if (ret == -1) 
    {
        return -1;
    }
    ret = VecGetValueByIndex(Heap->Array, Index, &value);
    if (ret == -1) 
    {
        return -1;
    }
    if (Heap->isMax == 0)
    {
        if (left != -1 && leftValue < value)
        {
            better = left;
            value = leftValue;
        }
        if (right != -1 && rightValue < value)
        {
            better = right;
        }
    }
    else
    {
        if (left != -1 && leftValue > value)
        {
            better = left;
            value = leftValue;
        }
        if (right != -1 && rightValue > value)
        {
            better = right;
        }
    }
    if (better != Index)
    {
        Swap(Heap, Index, better);
        return HeapifyDown2(Heap, better);
    }
    return 0;
}

int HpRemove(CC_HEAP *Heap, int Value)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Heap == NULL)
    {
        return -1;
    }
    int count = VecGetCount(Heap->Array);
    int temp;
    int ret;
    if (count == -1)
    {
        return -1;
    }
    for (int i = 0; i < count; i++)
    {
        ret = VecGetValueByIndex(Heap->Array, i, &temp);
        if (ret == -1)
        {
            return -1;
        }
        if (temp == Value)
        {
            ret = Swap(Heap, i, count - 1);
            if (ret == -1)
            {
                return -1;
            }
            ret = VecRemoveByIndex(Heap->Array, count - 1);
            if (ret == -1)
            {
                return -1;
            }
            HeapifyDown2(Heap, i);
            i = 0;
            count--;
        }

    }
        
    return 0;
}

int HpGetExtreme(CC_HEAP *Heap, int* ExtremeValue)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    CC_UNREFERENCED_PARAMETER(ExtremeValue);
    if (Heap == NULL)
    {
        return -1;
    }
    if (ExtremeValue == NULL)
    {
        return -1;
    }
    int ret, value;
    ret = VecGetValueByIndex(Heap->Array, 0, &value);
    if (ret == -1)
    {
        return -1;
    }
    *ExtremeValue = value;
    return 0;
}

int HpPopExtreme(CC_HEAP *Heap, int* ExtremeValue)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    CC_UNREFERENCED_PARAMETER(ExtremeValue);
    if (Heap == NULL)
    {
        return -1;
    }
    if (ExtremeValue == NULL)
    {
        return -1;
    }
    int ret, value;
    ret = VecGetValueByIndex(Heap->Array, 0, &value);
    if (ret == -1)
    {
        return -1;
    }
    *ExtremeValue = value;
    ret = HpRemove(Heap, value);
    if (ret == -1)
    {
        return -1;
    }
    return 0;
}

int HpGetElementCount(CC_HEAP *Heap)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    if (Heap == NULL)
    {
        return -1;
    }
    int ret;
    ret = VecGetCount(Heap->Array);
    return ret; // -1 or the count
}

int HpSortToVector(CC_HEAP *Heap, CC_VECTOR* SortedVector)
{
    CC_UNREFERENCED_PARAMETER(Heap);
    CC_UNREFERENCED_PARAMETER(SortedVector);
    if (Heap == NULL)
    {
        return -1;
    }
    if (SortedVector == NULL)
    {
        return -1;
    }
    int count, value, temp;
    int ret;
    count = HpGetElementCount(Heap);
    while (count > 0)
    {
        if (count == -1)
        {
            return -1;
        }
        ret = HpPopExtreme(Heap, &value);
        if (ret == -1)
        {
            return -1;
        }
        if (Heap->isMax == 0)
            VecInsertTail(SortedVector, value);
        else VecInsertHead(SortedVector, value);
        count--;
    }
    count = VecGetCount(SortedVector);
    for (int i = 0; i < count; i++)
    {
        VecGetValueByIndex(SortedVector, i, &temp);
        HpInsert(Heap, temp);
    }
    return 0;
}
