#ifndef BST_H_
#define BST_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct Node {
  struct Node *left, *right;
  size_t size;
  char data[];
} Node_t;

typedef const void *GetKeyFunc_t(const void *data);

typedef int CmpFunc_t(const void *key1, const void *key2);

typedef struct {
  struct Node *root;
  CmpFunc_t *cmp;
  GetKeyFunc_t *getKey;
} BST_t;

BST_t *newBST(CmpFunc_t *cmp, GetKeyFunc_t *getKey);

_Bool BST_insert(BST_t *bst, const void *data, size_t size);

const void *BST_search(BST_t *bst, const void *key);

_Bool BST_erase(BST_t *bst, const void *key);

void BST_clear(BST_t *bst);

int BST_inorder( BST_t *bst, _Bool (*action)( void *data ));
//int BST_rev_inorder( BST_t *pBST, _Bool (*action)( void *pData ));
//int BST_preorder( BST_t *pBST, _Bool (*action)( void *pData ));
//int BST_postorder( BST_t *pBST, _Bool (*action)( void *pData ));


#endif // BST_H_
