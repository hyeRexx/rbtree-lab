#include "rbtree.h"

#include <stdlib.h>

// set node data
void set_node(node_t *new, key_t key, color_t color, node_t *parent,
              node_t *left, node_t *right) {
  new->key = key;
  new->color = color;
  new->parent = parent;
  new->left = left;
  new->right = right;

  return;
}

// rotation
void rotate_left(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;

  if (y->left != t->nil) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
  return;
}

void rotate_right(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;

  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
  return;
}

// after insert :
void insert_fixup(rbtree *t, node_t *n) {
  node_t *uncle;
  while (n->parent->color == RBTREE_RED) {  // case3, case2 > case3, case1
    if (n->parent == n->parent->parent->left) {
      uncle = n->parent->parent->right;

      if (uncle->color == RBTREE_RED) {  // case 1
        n->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        uncle->parent->color = RBTREE_RED;  // n->p->p->color
        n = n->parent->parent;              // grandpa color check
      } else {
        if (n == n->parent->right) {
          n = n->parent;
          rotate_left(t, n);
        }
        n->parent->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        rotate_right(t, n->parent->parent);
      }
    } else {  //위에거 반대로~~~
      uncle = n->parent->parent->left;

      if (uncle->color == RBTREE_RED) {  // case 1
        n->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        uncle->parent->color = RBTREE_RED;  // n->p->p->color
        n = n->parent->parent;              // grandpa color check
      } else {
        if (n == n->parent->left) {
          n = n->parent;
          rotate_right(t, n);
        }
        n->parent->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        rotate_left(t, n->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

rbtree *new_rbtree(void) {  // TODO: initialize struct if needed
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *SENTINEL = (node_t *)calloc(1, sizeof(node_t));  // setinel node
  SENTINEL->color = RBTREE_BLACK;
  t->nil = SENTINEL;
  t->root = SENTINEL;

  return t;
}

void delete_rbtree_node(rbtree *t, node_t *n) {
  node_t *pNode = n;
  if (pNode !=
      t->nil) {  // 들어가기, 부모노드의 포인터부터 해제, 리프노드 메모리 해제.
    delete_rbtree_node(t, pNode->left);
    delete_rbtree_node(t, pNode->right);
    if (pNode->parent->left == pNode) {
      pNode->parent->left = t->nil;
    } else if (pNode->parent->right == pNode) {
      pNode->parent->right = t->nil;
    }
    free(pNode);
    pNode = NULL;
  }
}

void delete_rbtree(rbtree *t) {  // TODO: reclaim the tree nodes's memory
  delete_rbtree_node(t, t->root);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {  // TODO: implement insert
  node_t *new = (node_t *)calloc(1, sizeof(node_t));
  node_t *end = t->nil;
  node_t *start = t->root;

  // if(start == t->nil) {
  //   set_node(new, key, RBTREE_BLACK, end, t->nil, t->nil);
  //   t->root = new;
  //   return t->root;
  // }

  while (start != t->nil) {
    end = start;
    if (key < start->key) {
      start = start->left;
    } else {
      start = start->right;
    }
  }
  if (end == t->nil) {  // 부모 노드 처리
    t->root = new;
  } else if (end->key > key) {
    end->left = new;
  } else {
    end->right = new;
  }
  set_node(new, key, RBTREE_RED, end, t->nil, t->nil);
  insert_fixup(t, new);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {  // TODO: implement find
  node_t *pNode = t->root;
  while (pNode != t->nil) {
    if (key == pNode->key) {
      return pNode;
    } else if (key < pNode->key) {
      pNode = pNode->left;
    } else {
      pNode = pNode->right;
    }
  }
  pNode = NULL;
  return pNode;
}

node_t *rbtree_min(const rbtree *t) {  // TODO: implement find
  node_t *pNode = t->root;
  while (pNode->left != t->nil) {
    pNode = pNode->left;
  }
  return pNode;
}

node_t *rbtree_max(const rbtree *t) {  // TODO: implement find
  node_t *pNode = t->root;
  while (pNode->right != t->nil) {
    pNode = pNode->right;
  }
  return pNode;
}

// 자체
void rbtree_transplant(rbtree *t, node_t *pNodeDel, node_t *pNode) {
  if (pNodeDel->parent == t->nil) {
    t->root = pNode;
  } else if (pNodeDel == pNodeDel->parent->left) {
    pNodeDel->parent->left = pNode;
  } else {
    pNodeDel->parent->right = pNode;
  }
  pNode->parent = pNodeDel->parent;
  return;
}

// 자체
node_t *find_successor(rbtree *t, node_t *pNode) {
  pNode = pNode->right;
  while (pNode->left != t->nil) {
    pNode = pNode->left;
  }
  return pNode;
}

// 자체
void delete_fixup(rbtree *t, node_t *pExtra) {
  while (pExtra != t->root && pExtra->color == RBTREE_BLACK) {
    if (pExtra == pExtra->parent->left) {
      node_t *pNode = pExtra->parent->right;

      if (pNode->color == RBTREE_RED) {
        pNode->color = RBTREE_BLACK;
        pExtra->parent->color = RBTREE_RED;
        rotate_left(t, pExtra->parent);
        pNode = pExtra->parent->right;
      }

      if (pNode->left->color == RBTREE_BLACK &&
          pNode->right->color == RBTREE_BLACK) {
        pNode->color = RBTREE_RED;
        pExtra = pExtra->parent;
      }

      else {
        if (pNode->right->color == RBTREE_BLACK) {
          pNode->left->color = RBTREE_BLACK;
          pNode->color = RBTREE_RED;
          rotate_right(t, pNode);
          pNode = pExtra->parent->right;
        }

        pNode->color = pExtra->parent->color;
        pExtra->parent->color = RBTREE_BLACK;
        pNode->right->color = RBTREE_BLACK;
        rotate_left(t, pExtra->parent);
        pExtra = t->root;
      }
    }

    else {
      node_t *pNode = pExtra->parent->left;

      if (pNode->color == RBTREE_RED) {
        pNode->color = RBTREE_BLACK;
        pExtra->parent->color = RBTREE_RED;
        rotate_right(t, pExtra->parent);
        pNode = pExtra->parent->left;
      }
      // pnode = sbl
      if (pNode->right->color == RBTREE_BLACK &&
          pNode->left->color == RBTREE_BLACK) {
        pNode->color = RBTREE_RED;
        pExtra = pExtra->parent;
      }

      else {
        if (pNode->left->color == RBTREE_BLACK) {
          pNode->right->color = RBTREE_BLACK;
          pNode->color = RBTREE_RED;
          rotate_left(t, pNode);
          pNode = pExtra->parent->left;
        }

        pNode->color = pExtra->parent->color;
        pExtra->parent->color = RBTREE_BLACK;
        pNode->left->color = RBTREE_BLACK;
        rotate_right(t, pExtra->parent);
        pExtra = t->root;
      }
    }
  }
  pExtra->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {  // TODO: implement erase
  node_t *pNode;
  node_t *pExtra;
  color_t originColor;

  pNode = p;
  originColor = pNode->color;

  if (p->left ==
      t->nil) {  //삭제하려는 노드 왼쪽 자식 없음 오른쪽 자식으로 갈아끼기
    pExtra = p->right;
    rbtree_transplant(t, p, p->right);
  } else if (p->right == t->nil) {  //오른쪽 자식 없음 왼쪽 자식으로 갈아끼기
    pExtra = p->left;
    rbtree_transplant(t, p, p->left);
  } else {  // 자식이 둘일때
    pNode = find_successor(t, p);
    originColor = pNode->color;
    pExtra = pNode->right;

    if (pNode->parent == p) {
      pExtra->parent = pNode;
    } else {
      rbtree_transplant(t, pNode, pNode->right);
      pNode->right = p->right;
      pNode->right->parent = pNode;
    }

    rbtree_transplant(t, p, pNode);
    pNode->left = p->left;
    pNode->left->parent = pNode;
    pNode->color = p->color;
  }

  // if(pExtra->color == RBTREE_BLACK) {
  //   printf("---1, %d, ::: %d\n", pExtra->key, p->right->key);
  // } else {
  //   printf("---0, %d, ::: %d\n", pExtra->key, p->right->key);
  // }

  if (originColor == RBTREE_BLACK) {
    delete_fixup(t, pExtra);
  }

  free(p);
  return 0;
}

void addArr(const rbtree *t, node_t *pNode, key_t *pArr, key_t *pIdx,
            const size_t n) {  //자체생성함수
  if (*pIdx <= n) {
    if (pNode->left != t->nil) {
      addArr(t, pNode->left, pArr, pIdx, n);
    }
    pArr[(*pIdx)++] = pNode->key;
    if (pNode->right != t->nil) {
      addArr(t, pNode->right, pArr, pIdx, n);
    }
  }
}

int rbtree_to_array(const rbtree *t, key_t *arr,
                    const size_t n) {  // TODO: implement to_array
  key_t *pIdx = (key_t *)calloc(1, sizeof(key_t));
  addArr(t, t->root, arr, pIdx, n);
  free(pIdx);
  return 0;
}