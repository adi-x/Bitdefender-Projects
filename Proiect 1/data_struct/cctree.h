#pragma once

typedef struct _CC_TREE CC_TREE;

int TreeCreate(CC_TREE **Tree); // ** Works and tested
int TreeDestroy(CC_TREE **Tree); // ** Works and tested

// Duplicates are allowed
int TreeInsert(CC_TREE *Tree, int Value); // ** Works and tested

// Removes all elements equal to Value
int TreeRemove(CC_TREE *Tree, int Value); // ** Works and tested


//  Returns:
//       1  - Tree contains Value
//       0  - Tree does not contain Value
//      -1  - Error or invalid parameter
int TreeContains(CC_TREE *Tree, int Value); // ** Works and tested

// Returns the number of elements in Tree or -1 in case of error or invalid parameter
int TreeGetCount(CC_TREE *Tree); // ** Works and tested

// Returns the height of Tree or -1 in case of error or invalid parameter
int TreeGetHeight(CC_TREE *Tree); // ** Works and tested

// Removes every element of the tree
int TreeClear(CC_TREE *Tree); // ** Works and tested

int TreeGetNthPreorder(CC_TREE *Tree, int Index, int *Value); // ** Works and tested
int TreeGetNthInorder(CC_TREE *Tree, int Index, int *Value); // ** Works and tested
int TreeGetNthPostorder(CC_TREE *Tree, int Index, int *Value); // ** Works and tested
