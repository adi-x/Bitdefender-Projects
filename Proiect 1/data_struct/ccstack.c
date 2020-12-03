#include "ccstack.h"
#include "common.h"

typedef struct _CC_StackItem
{
    int Value;
    struct _CC_StackItem* Next;
    struct _CC_StackItem* Previous;
} CC_StackItem;

typedef struct _CC_STACK {
    // Members
    CC_StackItem* First;
    CC_StackItem* Last;
    int Count;
} CC_STACK;


int StCreate(CC_STACK **Stack)
{
    CC_UNREFERENCED_PARAMETER(Stack);

    CC_STACK* newStack = NULL;

    if (Stack == NULL)
    {
        return -1;
    }

    newStack = malloc(sizeof(CC_STACK));

    if (newStack == NULL)
    {
        return -1;
    }

    CC_StackItem* NextElement = NULL;
    NextElement = malloc(sizeof(CC_StackItem));
    if (NextElement == NULL)
    {
        return -1;
    }
    NextElement->Next = NULL;
    NextElement->Previous = NULL;

    newStack->First = NextElement;
    newStack->Last = NextElement;
    newStack->Count = 0;

    *Stack = newStack;

    return 0;
}

int StDestroy(CC_STACK **Stack)
{
    CC_UNREFERENCED_PARAMETER(Stack);

    CC_STACK* stack = *Stack;

    if (Stack == NULL)
    {
        return -1;
    }

    CC_StackItem* Item;
    CC_StackItem* Aux;
    Item = stack->First;
    while (Item->Next != NULL)
    {
        Aux = Item->Next;
        free(Item);
        Item = Aux;
    }

    free(Item);
    free(stack);

    *Stack = NULL;

    return 0;
}

int StPush(CC_STACK *Stack, int Value)
{
    CC_UNREFERENCED_PARAMETER(Stack);
    CC_UNREFERENCED_PARAMETER(Value);

    if (Stack == NULL)
    {
        return -1;
    }

    CC_StackItem* emptySpace = NULL;
    emptySpace = malloc(sizeof(CC_StackItem));
    if (emptySpace == NULL)
    {
        return -1;
    }
    emptySpace->Previous = Stack->Last;
    emptySpace->Next = NULL;

    Stack->Last->Value = Value;
    Stack->Last->Next = emptySpace;
    Stack->Last = Stack->Last->Next;
    Stack->Count++;

    return 0;
}

int StPop(CC_STACK *Stack, int *Value)
{
    CC_UNREFERENCED_PARAMETER(Stack);
    CC_UNREFERENCED_PARAMETER(Value);

    if (Stack == NULL)
    {
        return -1;
    }

    if (Stack->Count <= 0)
    {
        return -1;
    }

    CC_StackItem* actualLast = Stack->Last->Previous; // Getting the Last valid item
    free(Stack->Last); // freeing the functional last item
    Stack->Last = actualLast; // last valid item becomes last functional item
    Stack->Last->Next = NULL; 
    Stack->Count--; 

    *Value = actualLast->Value;

    return 0;
}

int StPeek(CC_STACK *Stack, int *Value)
{
    CC_UNREFERENCED_PARAMETER(Stack);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Stack == NULL)
    {
        return -1;
    }

    if (Stack->Count <= 0)
    {
        return -1;
    }

    CC_StackItem* actualLast = Stack->Last->Previous; // Getting the Last valid item

    *Value = actualLast->Value;

    return 0;
}

int StIsEmpty(CC_STACK *Stack)
{
    CC_UNREFERENCED_PARAMETER(Stack);

    if (Stack == NULL)
    {
        return -1;
    }

    return Stack->Count == 0;
}

int StGetCount(CC_STACK *Stack)
{
    CC_UNREFERENCED_PARAMETER(Stack);
    if (Stack == NULL)
    {
        return -1;
    }

    return Stack->Count;
}

int StClear(CC_STACK *Stack)
{
    CC_UNREFERENCED_PARAMETER(Stack);

    if (Stack == NULL)
    {
        return -1;
    }

    CC_StackItem* item;
    CC_StackItem* aux;
    item = Stack->First;
    while (item->Next != NULL)
    {
        aux = item->Next;
        free(item);
        item = aux;
    }
    Stack->First = item;
    Stack->Last = item;
    item->Previous = NULL;
    item->Next = NULL;
    Stack->Count = 0;
    return 0;
}

int StPushStack(CC_STACK *Stack, CC_STACK *StackToPush)
{
    CC_UNREFERENCED_PARAMETER(Stack);
    CC_UNREFERENCED_PARAMETER(StackToPush);

    if (Stack == NULL)
    {
        return -1;
    }
    if (StackToPush == NULL)
    {
        return -1;
    }

    CC_StackItem* actualLast = Stack->Last->Previous;
    CC_StackItem* Last = Stack->Last;
    actualLast->Next = StackToPush->First;
    Stack->Last = StackToPush->Last;
    StackToPush->First = Last;
    StackToPush->Last = Last;
    Last->Next = NULL;
    Last->Previous = NULL;
    Stack->Count += StackToPush->Count;
    StackToPush->Count = 0;

    return 0;
}
