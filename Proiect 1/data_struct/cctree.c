#include "cctree.h"
#include "common.h"

// Red black tree
typedef struct _CC_TreeNode {
    int Value;
    struct _CC_TreeNode* Left;
    struct _CC_TreeNode* Right;
    struct _CC_TreeNode* Parent;
    int Color; // 0 = Black, 1 = Red
    int IsLeft;
} TreeNode;

typedef struct _CC_TREE {
    int Count;
    TreeNode* Root;
} CC_TREE;

int TreeCreate(CC_TREE **Tree)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    if (Tree == NULL)
    {
        return -1;
    }
    CC_TREE* tree = NULL;
    tree = malloc(sizeof(CC_TREE));
    if (tree == NULL)
    {
        return -1;
    }
    tree->Root = NULL;
    tree->Count = 0;
    *Tree = tree;
    return 0;
}

void RecursiveDeallocation(TreeNode* node)
{
    if (node == NULL)
    {
        return;
    }
    if (node->Left != NULL)
    {
        RecursiveDeallocation(node->Left);
    }
    if (node->Right != NULL)
    {
        RecursiveDeallocation(node->Right);
    }
    free(node);
}

int TreeDestroy(CC_TREE **Tree)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    if (Tree == NULL)
    {
        return -1;
    }
    CC_TREE* tree = *Tree;
    RecursiveDeallocation(tree->Root);
    free(tree);
    *Tree = NULL;
    return 0;
}

int TreeBSTInsert(TreeNode* nod, int Value, TreeNode** NewNode)
{
    if (NewNode == NULL)
    {
        return -1;
    }
    if (Value < nod->Value)
    {
        if (nod->Left == NULL)
        {
            TreeNode* newNode = NULL;
            newNode = malloc(sizeof(TreeNode));
            if (newNode == NULL)
            {
                return -1;
            }
            newNode->Left = NULL;
            newNode->Right = NULL;
            newNode->Value = Value;
            newNode->Color = 1;
            newNode->Parent = nod;
            newNode->IsLeft = 1;
            nod->Left = newNode;
            *NewNode = newNode;
            
        }
        else
        {
            return TreeBSTInsert(nod->Left, Value, NewNode);
        }
    }
    else
    {
        if (nod->Right == NULL)
        {
            TreeNode* newNode = NULL;
            newNode = malloc(sizeof(TreeNode));
            if (newNode == NULL)
            {
                return -1;
            }
            newNode->Left = NULL;
            newNode->Right = NULL;
            newNode->Value = Value;
            newNode->Color = 1;
            newNode->Parent = nod;
            newNode->IsLeft = 0;
            nod->Right = newNode;
            *NewNode = newNode;
        }
        else
        {
            return TreeBSTInsert(nod->Right, Value, NewNode);
        }
    }
    return 0;
}

void InsertCase1(CC_TREE* Tree, TreeNode* GrandParent, TreeNode* Parent)
{
    TreeNode* aux = NULL;
    if (GrandParent->Parent == NULL) // root!
    {
        Tree->Root = Parent;
        Parent->Parent = NULL;
    }
    else
    {
        aux = GrandParent->Parent;
        if (GrandParent->IsLeft == 1)
        {
            aux->Left = Parent;
        }
        else
        {
            aux->Right = Parent;
        }
        Parent->IsLeft = GrandParent->IsLeft;
        Parent->Parent = aux;
    }
    GrandParent->Left = Parent->Right;
    if (GrandParent->Left != NULL)
    {
        aux = GrandParent->Left;
        aux->IsLeft = 1;
        aux->Parent = GrandParent;
    }
        
    GrandParent->Parent = Parent;
    GrandParent->IsLeft = 0;
    Parent->Color = 0;
    Parent->Right = GrandParent;
    GrandParent->Color = 1;
}

void InsertCase2(CC_TREE* Tree, TreeNode* GrandParent, TreeNode* Parent, TreeNode* Node)
{
    GrandParent->Left = Node;
    Node->Parent = GrandParent;
    Parent->Right = Node->Left;
    if(Parent->Right != NULL)
    {
        Parent->Right->IsLeft = 0;
        Parent->Right->Parent = Parent;
    }
    Parent->Parent = Node;
    Node->Left = Parent;
    Node->IsLeft = 1;
    Parent->IsLeft = 1;
    InsertCase1(Tree, GrandParent, Node);
}

void InsertCase3(CC_TREE* Tree, TreeNode* GrandParent, TreeNode* Parent)
{
    TreeNode* aux = NULL;
    if (GrandParent->Parent == NULL) // root!
    {
        Tree->Root = Parent;
        Parent->Parent = NULL;
    }
    else
    {
        aux = GrandParent->Parent;
        if (GrandParent->IsLeft == 1)
        {
            aux->Left = Parent;
        }
        else
        {
            aux->Right = Parent;
        }
        Parent->IsLeft = GrandParent->IsLeft;
        Parent->Parent = aux;
    }
    GrandParent->Right = Parent->Left;
    if (GrandParent->Right != NULL)
    {
        aux = GrandParent->Right;
        aux->IsLeft = 0;
        aux->Parent = GrandParent;
    }
    GrandParent->Parent = Parent;
    GrandParent->IsLeft = 1;
    Parent->Color = 0;
    Parent->Left = GrandParent;
    GrandParent->Color = 1;
}

void InsertCase4(CC_TREE* Tree, TreeNode* GrandParent, TreeNode* Parent, TreeNode* Node)
{
    GrandParent->Right = Node;
    Node->Parent = GrandParent;
    Parent->Left = Node->Right;
    if (Parent->Left != NULL)
    {
        Parent->Left->IsLeft = 1;
        Parent->Left->Parent = Parent;
    }
    Parent->Parent = Node;
    Node->Right = Parent;
    Node->IsLeft = 0;
    Parent->IsLeft = 0;
    InsertCase3(Tree, GrandParent, Node);
}

void Insert(CC_TREE* Tree, TreeNode* Parent, TreeNode* Node)
{
    if (Parent == NULL)
    {
        Node->Color = 0;
    }
    else if (Parent->Color == 1) // Parent is red (so not root!)
    {
        TreeNode* uncle = NULL;
        TreeNode* grandParent = NULL;
        grandParent = Parent->Parent;
        if (Parent->IsLeft == 1)
        {
            uncle = grandParent->Right;
        }
        else
        {
            uncle = grandParent->Left;
        }
        if (uncle != NULL && uncle->Color == 1)
        {
            uncle->Color = 0;
            Parent->Color = 0;
            grandParent->Color = 1;
            Insert(Tree, grandParent->Parent, grandParent);
        }
        else
        {
            if (Node->IsLeft == 1 && Parent->IsLeft == 1)
            {
                InsertCase1(Tree, grandParent, Parent);
            }
            else if (Node->IsLeft == 0 && Parent->IsLeft == 1)
            {
                InsertCase2(Tree, grandParent, Parent, Node);
            }
            else if (Node->IsLeft == 0 && Parent->IsLeft == 0)
            {
                InsertCase3(Tree, grandParent, Parent);
            }
            else if (Node->IsLeft == 1 && Parent->IsLeft == 0)
            {
                InsertCase4(Tree, grandParent, Parent, Node);
            }
        }
    }
}

int TreeInsert(CC_TREE *Tree, int Value)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Tree == NULL)
    {
        return -1;
    }
    if (Tree->Root == NULL)
    {
        TreeNode* newNode = NULL;
        newNode = malloc(sizeof(TreeNode));
        if (newNode == NULL)
        {
            return -1;
        }
        newNode->Left = NULL;
        newNode->Right = NULL;
        newNode->Value = Value;
        newNode->Color = 0;
        newNode->Parent = NULL;
        newNode->IsLeft = 0; // doesn't matter for root.
        Tree->Root = newNode;
    }
    else 
    {
        TreeNode* newNode = NULL;
        TreeBSTInsert(Tree->Root, Value, &newNode);
        TreeNode* parent = newNode->Parent;
        Insert(Tree, parent, newNode);
    }
    Tree->Count++;
    return 0;
}

void TreeFix(CC_TREE* Tree, TreeNode* p)
{
    // p the node to be resolved
    // z the node's parent
    // y the node's sibling
    // x the sibling's child
    //     z 
    //    / \
    //   y   p
    //  /
    // x

    TreeNode *x = NULL, *y = NULL, *z = NULL;
    int cx = 0, cy, cz, cp; // colors
    cp = p->Color;
    z = p->Parent;
    cz = z->Color;
    cy = 0;
    if (p->IsLeft == 1)
    {
        y = z->Right;
    }
    else 
    {
        y = z->Left;
    }
    if (y != NULL)
    {
        cy = y->Color;
        TreeNode* temp;
        temp = y->Left;
        if (temp != NULL) // Store left; preferably red
        {
            x = temp;
            if (temp->Color == 1)
            {
                cx = 1;
            }
        }
        temp = y->Right;
        if (temp != NULL) // if right is red or left doesn't exist store right;
        {
            if (temp->Color == 1)
            {
                x = temp;
                cx = 1;
            }
            else {
                if (x == NULL)
                {
                    x = temp;
                    cx = 0;
                }
            }
        }
    }
    if (cy == 0 && cx == 1)
    {
        // Case 1)
        // Black sibling with red child
        TreeNode* parent = z->Parent; // the grandparent
        TreeNode* aux = NULL;
        // y becomes the new parent
        z->Parent = y;
        y->Parent = parent;
        int zIsLeft = z->IsLeft;
        if (y->IsLeft == 1)
        {
            // Left cases...
            
            if ((y->Left != NULL && y->Right != NULL && y->Left->Color == 1 && y->Right->Color == 1) || (y->Left != NULL && y->Left->Color == 1)) // Left Left
            {
                aux = y->Left;
                aux->Color = 0;
                z->Left = y->Right;
                aux = z->Left;
                if (aux != NULL)
                {
                    aux->Parent = z;
                    aux->IsLeft = 1;
                }
                y->Right = z;
                z->IsLeft = 0;
            }
            else { // Left Right
                aux = y->Right;
                y->Right = y->Left;
                y->Left = aux;
                if (y->Right != NULL)
                    y->Right->IsLeft = 0;
                if (y->Left != NULL)
                {
                    y->Left->IsLeft = 1;
                    int aux3 = y->Left->Value;
                    y->Left->Value = y->Value;
                    y->Value = aux3;
                    z->Left = y->Right;
                }
                aux = z->Left;
                if (aux != NULL)
                {
                    aux->Parent = z;
                    aux->IsLeft = 1;
                }
                y->Right = z;
                z->IsLeft = 0;
                
            }
        }
        else {
            
            if ((y->Left != NULL && y->Right != NULL && y->Left->Color == 1 && y->Right->Color == 1) || (y->Right != NULL && y->Right->Color == 1)) // Right Right
            {
                aux = y->Right;
                aux->Color = 0;
                z->Right = y->Left;
                aux = z->Right;
                if (aux != NULL)
                {
                    aux->Parent = z;
                    z->Right->IsLeft = 0;
                }
                y->Left = z;
                z->IsLeft = 1;
            }
            else { // Left Right
                aux = y->Right;
                y->Right = y->Left;
                y->Left = aux;
                if (y->Left != NULL)
                    y->Left->IsLeft = 1;
                if (y->Right != NULL)
                {
                    y->Right->IsLeft = 0;
                    int aux3 = y->Right->Value;
                    y->Right->Value = y->Value;
                    y->Value = aux3;
                    z->Right = y->Left;
                }
                aux = z->Right;
                if (aux != NULL)
                {
                    aux->Parent = z;
                    z->Right->IsLeft = 0;
                }
                y->Left = z;
                z->IsLeft = 1;
            }
        }
        y->IsLeft = zIsLeft;
        y->Color = z->Color;
        x->Color = 0;
        z->Color = 0;
        p->Color = 0;
        // Connect y back to the grandparent
        if (parent == NULL) // z was root
            Tree->Root = y;
        else {
            if (y->IsLeft == 0)
            {
                parent->Right = y;
            }
            else {
                parent->Left = y;
            }
        }
    }
    else if (cy == 0 && cx == 0)
    {
        p->Color = 0;
        if (y != NULL)
            y->Color = 1;
        if (cz == 1)
            z->Color = 0;
        else if (z->Parent != NULL)
            TreeFix(Tree, z);
    }
    else if (cy == 1)
    {
        TreeNode* parent = z->Parent;
        z->Parent = y;
        y->Parent = parent;

        int zIsLeft = z->IsLeft;
        if (y->IsLeft == 1)
        {
            z->Left = y->Right;
            if (z->Left != NULL)
            {
                z->Left->Parent = z;
                z->Left->IsLeft = 1;
            }
            y->Right = z;
            z->IsLeft = 0;
        }
        else {
            z->Right = y->Left;
            if (z->Right != NULL)
            {
                z->Right->Parent = z;
                z->Right->IsLeft = 0;
            }
            y->Left = z;
            z->IsLeft = 1;
        }
        y->IsLeft = zIsLeft;
        y->Color = 0;
        z->Color = 1;
        if (parent == NULL) // z was root
        {
            Tree->Root = y;
            y->Parent = NULL;
        }
        else {
            if (y->IsLeft == 0)
            {
                parent->Right = y;
            }
            else {
                parent->Left = y;
            }
        }
        TreeFix(Tree, p);
    }
}

TreeNode* InorderSuccesor(TreeNode* Node)
{
    if (Node->Left != NULL)
        return InorderSuccesor(Node->Left);
    return Node;
}

TreeNode* InorderPredecessor(TreeNode* Node)
{
    if (Node->Right != NULL)
        return InorderPredecessor(Node->Right);
    return Node;
}


TreeNode* TreeBSTRemove(TreeNode* Node)
{
    if (Node == NULL) return NULL;
    TreeNode* left = NULL;
    TreeNode* right = NULL;
    left = Node->Left;
    right = Node->Right;
    if (left == NULL && right == NULL)
    {
        // No children case
        if (Node->Color == 0)
        {
            return Node;
        }
        else
        {
            // If the node is red then we simply remove it :)
            TreeNode* parent = NULL;
            parent = Node->Parent;
            if (Node->IsLeft == 1)
            {
                parent->Left = NULL;
            }
            else 
            {
                parent->Right = NULL;
            }
            free(Node);
            return NULL;
        }
    }
    else {
        // One child cases
        if (left != NULL && right == NULL)
        {
            TreeNode* parent = NULL;
            parent = Node->Parent;
            int color = 0;
            int nodeColor = 0;
            nodeColor = Node->Color;
            if (parent->Left == Node)
            {
                parent->Left = left;
                left->Parent = parent;
                color = left->Color;
            }
            else if (parent->Right == Node)
            {
                parent->Right = left;
                left->Parent = parent;
                color = left->Color;
            }
            left->IsLeft = Node->IsLeft;
            free(Node);
            Node = NULL;
            if (color == 0 || nodeColor == 0)
            {
                left->Color = 0;
            }
            if (color == 0 && nodeColor == 0)
            {
                return left;
            }
            else
            {
                return NULL;
            }
            
        }
        else if (left == NULL && right != NULL)
        {
            TreeNode* parent = NULL;
            parent = Node->Parent;
            int color = 0;
            int nodeColor = 0;
            nodeColor = Node->Color;
            if (parent->Left == Node)
            {
                parent->Left = right;
                right->Parent = parent;
                color = right->Color;
            }
            else if (parent->Right == Node)
            {
                parent->Right = right;
                right->Parent = parent;
                color = right->Color;
            }
            right->IsLeft = Node->IsLeft;
            free(Node);
            Node = NULL;
            if (color == 0 || nodeColor == 0)
            {
                right->Color = 0;
            }
            if (color == 0 && nodeColor == 0)
            {
                return right;
            }
            else
            {
                return NULL;
            }
        }
        else {
            // Both children case
            TreeNode* temp = InorderPredecessor(left);
            int aux = Node->Value;
            Node->Value = temp->Value;
            temp->Value = aux;
            return TreeBSTRemove(temp);
        }
    }
}


TreeNode* Find(TreeNode* Root, int Value)
{
    if (Root == NULL)
    {
        return NULL;
    }
    if (Root->Value == Value)
    {
        return Root;
    }
    if (Value < Root->Value)
    {
        return Find(Root->Left, Value);
    }
    else 
    {
        return Find(Root->Right, Value);
    }
}


int TreeRemove(CC_TREE *Tree, int Value)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Tree == NULL)
    {
        return -1;
    }
    TreeNode* nodeValue = Find(Tree->Root, Value);
    while (nodeValue != NULL)
    {
        Tree->Count--;
        nodeValue = TreeBSTRemove(nodeValue);
        if (nodeValue == Tree->Root)
        {
            free(nodeValue);
            Tree->Root = NULL;
            break;
        }
        if (nodeValue != NULL)
        {
            TreeFix(Tree, nodeValue);
            TreeNode* parent = nodeValue->Parent;
            if (parent != NULL)
            {
                if (nodeValue->IsLeft == 0)
                {
                    parent->Right = NULL;
                }
                if (nodeValue->IsLeft == 1)
                {
                    parent->Left = NULL;
                }
                free(nodeValue);
            }
        }

        nodeValue = Find(Tree->Root, Value);
    }
    
    return 0;
}

int TreeContains(CC_TREE *Tree, int Value)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Tree == NULL)
    {
        return -1;
    }
    if (Find(Tree->Root, Value) != NULL)
    {
        return 1;
    }
    return 0;
}

int TreeGetCount(CC_TREE *Tree)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    if (Tree == NULL)
    {
        return -1;
    }
    return Tree->Count;
}

int GetHeight(TreeNode* node)
{
    if (node != NULL)
    {
        int a = GetHeight(node->Left);
        int b = GetHeight(node->Right);
        return (a > b ? a : b) + 1;
    }
    return 0;
}

int TreeGetHeight(CC_TREE *Tree)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    if (Tree == NULL)
    {
        return -1;
    }
    return GetHeight(Tree->Root);
}

int TreeClear(CC_TREE *Tree)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    if (Tree == NULL)
    {
        return -1;
    }
    RecursiveDeallocation(Tree->Root);
    Tree->Root = NULL;
    Tree->Count = 0;
    return 0;
}

void preorder(TreeNode* node, int* Index, int* Value)
{
    if (node != NULL)
    {
        (*Index)--;
        if (*Index == 0)
        {
            *Value = node->Value;
        }
        else
        {
            preorder(node->Left, Index, Value);
            preorder(node->Right, Index, Value);
        }
    }
}
void inorder(TreeNode* node, int* Index, int* Value)
{
    if (node != NULL)
    {
        inorder(node->Left, Index, Value);
        (*Index)--;
        if (*Index == 0)
        {
            *Value = node->Value;
        }
        else
        {
            inorder(node->Right, Index, Value);
        }
    }
}

void postorder(TreeNode* node, int* Index, int* Value)
{
    if (node != NULL)
    {
        postorder(node->Left, Index, Value);
        postorder(node->Right, Index, Value);
        (*Index)--;
        if (*Index == 0)
        {
            *Value = node->Value;
        }
    }
}

int TreeGetNthPreorder(CC_TREE *Tree, int Index, int *Value)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    CC_UNREFERENCED_PARAMETER(Index);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Tree == NULL)
    {
        return -1;
    }
    if (Value == NULL)
    {
        return -1;
    }
    if (Index >= Tree->Count || Index < 0)
    {
        return -1;
    }
    Index++;
    preorder(Tree->Root, &Index, Value);
    return 0;
}

int TreeGetNthInorder(CC_TREE *Tree, int Index, int *Value)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    CC_UNREFERENCED_PARAMETER(Index);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Tree == NULL)
    {
        return -1;
    }
    if (Value == NULL)
    {
        return -1;
    }
    if (Index >= Tree->Count || Index < 0)
    {
        return -1;
    }
    Index++;
    inorder(Tree->Root, &Index, Value);
    return 0;
}

int TreeGetNthPostorder(CC_TREE *Tree, int Index, int *Value)
{
    CC_UNREFERENCED_PARAMETER(Tree);
    CC_UNREFERENCED_PARAMETER(Index);
    CC_UNREFERENCED_PARAMETER(Value);
    if (Tree == NULL)
    {
        return -1;
    }
    if (Value == NULL)
    {
        return -1;
    }
    if (Index >= Tree->Count || Index < 0)
    {
        return -1;
    }
    Index++;
    postorder(Tree->Root, &Index, Value);
    return 0;
}

