#include "rbtree.h"

#include <stdlib.h>

// [added] set node data
void set_node(node_t *pNode, key_t key, color_t color, node_t *parent,
              node_t *left, node_t *right) {
  pNode->key = key;
  pNode->color = color;
  pNode->parent = parent;
  pNode->left = left;
  pNode->right = right;

  return;
}

// initialize rbTree
rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *SENTINEL = (node_t *)calloc(1, sizeof(node_t));  // 센티넬 노드 사용
  SENTINEL->color = RBTREE_BLACK;
  t->nil = SENTINEL;
  t->root = SENTINEL;

  return t;
}

// [added] rotation - left : insert fixup, delete fixup
void rotate_left(rbtree *t, node_t *pNode) {
  node_t *pR = pNode->right;  // pNode = x, pR = y
  pNode->right = pR->left;    // pNode에서 pR 분리

  // pR의 왼쪽 자식 pNode에 연결 (BST: 왼쪽이 자신보다 작음)
  if (pR->left != t->nil) {
    pR->left->parent = pNode;
  }

  // parent 처리
  pR->parent = pNode->parent;
  if (pNode->parent == t->nil) {
    t->root = pR;
  } else if (pNode == pNode->parent->left) {
    pNode->parent->left = pR;
  } else {
    pNode->parent->right = pR;
  }

  // pR - pNode 부모 자식 관계 처리
  pR->left = pNode;
  pNode->parent = pR;

  return;
}

// [added] rotation - right : insert fixup, delete fixup
void rotate_right(rbtree *t, node_t *pNode) {
  node_t *pL = pNode->left;  // pNode = x, pL = y
  pNode->left = pL->right;   // pNode에서 pL 분리

  // pR의 왼쪽 자식 pNode에 연결 (BST: 왼쪽이 자신보다 작음)
  if (pL->right != t->nil) {
    pL->right->parent = pNode;
  }

  // parent 처리
  pL->parent = pNode->parent;
  if (pNode->parent == t->nil) {
    t->root = pL;
  } else if (pNode == pNode->parent->left) {
    pNode->parent->left = pL;
  } else {
    pNode->parent->right = pL;
  }

  // pL - pNode 부모 자식 관계 처리
  pL->right = pNode;
  pNode->parent = pL;

  return;
}

// [added] fixup after insert
void insert_fixup(rbtree *t, node_t *pNode) {
  node_t *pUncle;

  // 부모가 RED인 동안
  while (pNode->parent->color == RBTREE_RED) {
    // LEFT : 부모가 왼쪽 자식
    if (pNode->parent == pNode->parent->parent->left) {
      pUncle = pNode->parent->parent->right;  // 삼촌 노드 초기화

      // Case 1 : 부모 RED, 삼촌 RED
      if (pUncle->color == RBTREE_RED) {
        pNode->parent->color = RBTREE_BLACK;  // 부모 : BLACK
        pUncle->color = RBTREE_BLACK;         // 삼촌 : BLACK
        pUncle->parent->color = RBTREE_RED;   // 할아버지 : RED
        pNode = pNode->parent->parent;  // 검사 위치 할아버지로 변경
      } else {
        // Case 2 : 부모 RED, 꺾인 경로
        if (pNode == pNode->parent->right) {
          pNode = pNode->parent;
          rotate_left(t, pNode);
        }

        // Case 3 : 부모 RED, 인서트 - 부모 - 할아버지 일직선상
        pNode->parent->color = RBTREE_BLACK;        // 부모 : BLACK
        pNode->parent->parent->color = RBTREE_RED;  // 할아버지 : RED
        rotate_right(t, pNode->parent->parent);
      }

      // RIGHT : 부모가 오른쪽 자식
    } else {
      pUncle = pNode->parent->parent->left;

      if (pUncle->color == RBTREE_RED) {
        pNode->parent->color = RBTREE_BLACK;
        pUncle->color = RBTREE_BLACK;
        pUncle->parent->color = RBTREE_RED;
        pNode = pNode->parent->parent;
      } else {
        if (pNode == pNode->parent->left) {
          pNode = pNode->parent;
          rotate_right(t, pNode);
        }
        pNode->parent->color = RBTREE_BLACK;
        pNode->parent->parent->color = RBTREE_RED;
        rotate_left(t, pNode->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;

  return;
}

// insert new node
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *pNode = (node_t *)calloc(1, sizeof(node_t));
  node_t *end = t->nil;
  node_t *pSearch = t->root;

  // insert 위치 탐색, 빈 트리가 아닌 경우
  while (pSearch != t->nil) {
    end = pSearch;
    if (key < pSearch->key) {  // insert key < 탐색 위치 key : 왼쪽으로
      pSearch = pSearch->left;
    } else {  // insert key >= 탐색 위치 key : 오른쪽으로
      pSearch = pSearch->right;
    }
  }

  // insert 위치 도달
  if (end == t->nil) {
    t->root = pNode;
  } else if (end->key > key) {
    end->left = pNode;
  } else {
    end->right = pNode;
  }

  // set node data and fixup
  set_node(pNode, key, RBTREE_RED, end, t->nil, t->nil);
  insert_fixup(t, pNode);

  return t->root;
}

// [added] delete all nodes : delete_rbtree
void delete_all_node(rbtree *t, node_t *pNode) {
  if (pNode != t->nil) {               // 재귀 탈출 조건 : 경계 도달
    delete_all_node(t, pNode->left);   // 왼쪽 서브트리 제거
    delete_all_node(t, pNode->right);  // 오른쪽 서브트리 제거

    // 부모와 연결 해제
    if (pNode->parent->left == pNode) {
      pNode->parent->left = t->nil;
    } else if (pNode->parent->right == pNode) {
      pNode->parent->right = t->nil;
    }

    // 노드 메모리 해제 및 NULL 처리
    free(pNode);
    pNode = NULL;
  }

  return;
}

// clear tree
void delete_rbtree(rbtree *t) {
  delete_all_node(t, t->root);  // 개별 노드 제거

  free(t->nil);  // SENTINEL 해제
  t->nil = NULL;
  free(t);  // rbTree 해제
  t = NULL;

  return;
}

// find key in rbTree
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *pNode = t->root;

  // 경계에 도달하기 전까지 탐색
  while (pNode != t->nil) {
    if (key == pNode->key) {
      return pNode;  // 일치하는 키 값 있는 경우 pNode 리턴
    } else if (key < pNode->key) {
      pNode = pNode->left;
    } else {
      pNode = pNode->right;
    }
  }

  return NULL;  // 일치하는 키 값 없이 경계에 도달한 경우 NULL 리턴
}

// find min value
node_t *rbtree_min(const rbtree *t) {
  node_t *pNode = t->root;

  // 왼쪽으로 계속해서 탐색 (BST)
  while (pNode->left != t->nil) {
    pNode = pNode->left;
  }
  return pNode;
}

// find max value
node_t *rbtree_max(const rbtree *t) {
  node_t *pNode = t->root;

  // 오른쪽으로 계속해서 탐색 (BST)
  while (pNode->right != t->nil) {
    pNode = pNode->right;
  }
  return pNode;
}

// [added] transplant : change child (pBefore >>> pAfter)
void rbtree_transplant(rbtree *t, node_t *pBefore, node_t *pAfter) {
  // 삭제하는 위치로 이식 (부모 입장)
  if (pBefore->parent == t->nil) {
    t->root = pAfter;
  } else if (pBefore == pBefore->parent->left) {
    pBefore->parent->left = pAfter;
  } else {
    pBefore->parent->right = pAfter;
  }

  // 부모 설정 (자식 입장)
  pAfter->parent = pBefore->parent;
  return;
}

// [added] find del node's successor
node_t *find_successor(rbtree *t, node_t *pNode) {
  pNode = pNode->right;  // 오른쪽 서브트리에서 가장 작은 값(successor) 탐색
  while (pNode->left != t->nil) {
    pNode = pNode->left;
  }

  return pNode;
}

// [added] fixup after delete
void delete_fixup(rbtree *t, node_t *pExtra) {
  // pExtra(ExtraBlack)이 root이거나 RED이면 탈출
  while (pExtra != t->root && pExtra->color == RBTREE_BLACK) {
    // LEFT : pExtra가 왼쪽 자식
    if (pExtra == pExtra->parent->left) {
      node_t *pSbl = pExtra->parent->right;  // sibling node

      // Case 1 : 형제 RED
      if (pSbl->color == RBTREE_RED) {
        pSbl->color = RBTREE_BLACK;
        pExtra->parent->color = RBTREE_RED;  // 부모와 형제 색상 교체
        rotate_left(t, pExtra->parent);
        pSbl = pExtra->parent->right;
      }

      // Case 2 : 형제 BLACK, 형제의 두 자식 모두 BLACK
      if (pSbl->left->color == RBTREE_BLACK &&
          pSbl->right->color == RBTREE_BLACK) {
        pSbl->color = RBTREE_RED;  // 형제 : RED
        pExtra = pExtra->parent;   // pExtra를 부모로 교체
      }

      else {
        // Case 3 : 형제 BLACK, 형제의 왼쪽 자식 RED - 오른쪽 자식 BLACK
        if (pSbl->right->color == RBTREE_BLACK) {
          pSbl->left->color = RBTREE_BLACK;
          pSbl->color = RBTREE_RED;  // 형제 : RED
          rotate_right(t, pSbl);
          pSbl = pExtra->parent->right;
        }

        // Case 4 : 형제 BLACK, 형제의 오른쪽 자식 RED
        pSbl->color = pExtra->parent->color;
        pExtra->parent->color = RBTREE_BLACK;
        pSbl->right->color = RBTREE_BLACK;  // 형제 : BLACK
        rotate_left(t, pExtra->parent);
        pExtra = t->root;  // 탈출 조건 : pExtra = rootNode
      }
    }

    // RIGHT : pExtra가 오른쪽 자식
    else {
      node_t *pSbl = pExtra->parent->left;

      if (pSbl->color == RBTREE_RED) {
        pSbl->color = RBTREE_BLACK;
        pExtra->parent->color = RBTREE_RED;
        rotate_right(t, pExtra->parent);
        pSbl = pExtra->parent->left;
      }

      if (pSbl->right->color == RBTREE_BLACK &&
          pSbl->left->color == RBTREE_BLACK) {
        pSbl->color = RBTREE_RED;
        pExtra = pExtra->parent;
      }

      else {
        if (pSbl->left->color == RBTREE_BLACK) {
          pSbl->right->color = RBTREE_BLACK;
          pSbl->color = RBTREE_RED;
          rotate_left(t, pSbl);
          pSbl = pExtra->parent->left;
        }

        pSbl->color = pExtra->parent->color;
        pExtra->parent->color = RBTREE_BLACK;
        pSbl->left->color = RBTREE_BLACK;
        rotate_right(t, pExtra->parent);
        pExtra = t->root;
      }
    }
  }
  pExtra->color = RBTREE_BLACK;  // Extra 처리
}

int rbtree_erase(rbtree *t, node_t *p) {  // TODO: implement erase
  node_t *pNode = p;
  node_t *pExtra;
  color_t originColor = pNode->color;

  // case : 자식이 하나인 경우
  if (p->left == t->nil) {
    pExtra = p->right;
    rbtree_transplant(t, p, p->right);
  } else if (p->right == t->nil) {
    pExtra = p->left;
    rbtree_transplant(t, p, p->left);
  }

  // case : 자식이 둘인 경우
  else {
    pNode = find_successor(t, p);
    originColor = pNode->color;  // successor color 저장 (for pExtra)
    pExtra = pNode->right;

    // 지우려는 노드가 successor의 부모일 때
    if (pNode->parent == p) {
      pExtra->parent = pNode;
    } else {
      // 지우려는 노드가 successor의 부모가 아닐 때
      // pNode 부모에 pNode 오른쪽 서브트리 이식
      rbtree_transplant(t, pNode, pNode->right);
      pNode->right = p->right;
      pNode->right->parent = pNode;
    }

    // 지우려는 노드의 부모에 pNode 이식
    rbtree_transplant(t, p, pNode);
    pNode->left = p->left;
    pNode->left->parent = pNode;
    pNode->color = p->color;
  }

  // 실제로 삭제된 색이 BLACK인 경우 fix
  if (originColor == RBTREE_BLACK) {
    delete_fixup(t, pExtra);
  }

  // 삭제 노드 메모리 해제
  free(p);
  p = NULL;

  return 0;
}

// [added] rbtree to array : add arr
void addArr(const rbtree *t, node_t *pNode, key_t *pArr, key_t *pIdx,
            const size_t n) {
  if (*pIdx < n) {  // 주어진 사이즈보다 작을 경우에만 들어감
    if (pNode->left != t->nil) {
      addArr(t, pNode->left, pArr, pIdx, n);
    }
    pArr[(*pIdx)++] = pNode->key;
    if (pNode->right != t->nil) {
      addArr(t, pNode->right, pArr, pIdx, n);
    }
  }

  return;
}

// tree to array, inorder
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  key_t idx = 0;       // array index
  key_t *pIdx = &idx;  // array index pointer, 매개변수로 전달

  addArr(t, t->root, arr, pIdx, n);
  return 0;
}