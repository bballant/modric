#include "bst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const void *defaultGetKey(const void *data) { return data; }

/*
** Create Empty BST
*/
BST_t *newBST(CmpFunc_t *cmp, GetKeyFunc_t *getKey) {
  BST_t *bst = NULL;
  if (cmp != NULL) {
    bst = malloc(sizeof(BST_t));
  }
  if (bst != NULL) {
    bst->root = NULL;
    bst->cmp = cmp;
    bst->getKey = (getKey != NULL) ? getKey : defaultGetKey;
  }
  return bst;
}

static _Bool insert(BST_t *bst, Node_t **nnode, const void *data, size_t size);

_Bool BST_insert(BST_t *bst, const void *data, size_t size) {
  if (bst == NULL || data == NULL || size == 0) {
    return false;
  }
  return insert(bst, &(bst->root), data, size);
}

static _Bool insert(BST_t *bst, Node_t **nnode, const void *data, size_t size) {
  Node_t *node = *nnode;
  if (node == NULL) {
    node = malloc(sizeof(Node_t) + size);
    if (node != NULL) {
      node->left = node->right = NULL;
      memcpy(node->data, data, size);
      *nnode = node;
      return true;
    } else {
      return false;
    }
  } else {
    const void *key1 = bst->getKey(data);
    const void *key2 = bst->getKey(node->data);
    if (bst->cmp(key1, key2) < 0) {
      return insert(bst, &(node->left), data, size);
    } else {
      return insert(bst, &(node->right), data, size);
    }
  }
}

static const void *search(BST_t *bst, const Node_t *node, const void *key);

const void *BST_search(BST_t *bst, const void *key) {
  if (bst == NULL || key == NULL)
    return NULL;
  return search(bst, bst->root, key); // Start at the root of the
                                      // tree.
}
static const void *search(BST_t *bst, const Node_t *node, const void *key) {
  if (node == NULL)
    return NULL; // No subtree to search;
  // no match found.
  else { // Compare data:
    int cmp_res = bst->cmp(key, bst->getKey(node->data));
    if (cmp_res == 0) // Found a match.
      return node->data;
    else if (cmp_res < 0)                  // Continue the search
      return search(bst, node->left, key); // in the left subtree,
    else
      return search(bst, node->right, key); // or in the right
    // subtree.
  }
}

static Node_t *detachMin(Node_t **nnode) {
  Node_t *node = *nnode; // A pointer to the current node.
  if (node == NULL)
    return NULL; // pNode is an empty subtree.
  else if (node->left != NULL)
    return detachMin(&(node->left)); // The minimum is in the left
  // subtree.
  else {                  // pNode points to the minimum node.
    *nnode = node->right; // Attach the right child to the parent.
    return node;
  }
}

static _Bool erase(BST_t *bst, Node_t **nnode, const void *key);

_Bool BST_erase(BST_t *bst, const void *key) {
  if (bst == NULL || key == NULL)
    return false;
  return erase(bst, &(bst->root), key); // Start at the root of
}

static _Bool erase(BST_t *bst, Node_t **nnode, const void *key) {
  Node_t *node = *nnode; // Pointer to the current node.
  if (node == NULL)
    return false; // No match found.
  // Compare data:
  int cmp_res = bst->cmp(key, bst->getKey(node->data));
  if (cmp_res < 0)                         // Continue the search
    return erase(bst, &(node->left), key); // in the left subtree,
  else if (cmp_res > 0)
    return erase(bst, &(node->right), key); // or in the right
  // subtree.
  else {                    // Found the node to be deleted.
    if (node->left == NULL) // If no more than one child,
      *nnode = node->right; // attach the child to the parent.
    else if (node->right == NULL)
      *nnode = node->left;
    else // Two children: replace the node with
    {    // the minimum from the right subtree.
      Node_t *min = detachMin(&(node->right));
      *nnode = min;           // Graft it onto the deleted node's parent.
      min->left = node->left; // Graft the deleted node's children.
      min->right = node->right;
    }
    free(node); // Release the deleted node's storage.
    return true;
  }
}

static void clear(Node_t *node);

void BST_clear(BST_t *bst) {
  if (bst != NULL) {
    clear(bst->root);
    bst->root = NULL;
  }
}

static void clear(Node_t *node) {
  if (node != NULL) {
    clear(node->left);
    clear(node->right);
    free(node);
  }
}

static int inorder(Node_t *node, _Bool (*action)(void *data));

int BST_inorder(BST_t *bst, _Bool (*action)(void *data)) {
  if (bst == NULL || action == NULL)
    return 0;
  else
    return inorder(bst->root, action);
}

static int inorder(Node_t *node, _Bool (*action)(void *data)) {
  int count = 0;
  if (node == NULL)
    return 0;
  count = inorder(node->left, action); // L: Traverse the left
  // subtree.
  if (action(node->data))                // N: Visit the current node
    ++count;                             // itself.
  count += inorder(node->right, action); // R: Traverse the right
  // subtree.
  return count;
}
