#ifndef HELLO_METAL_RBTREE_H
#define HELLO_METAL_RBTREE_H

#include <stdbool.h>

struct rbtree_node {
  struct rbtree_node *left, *right;
  bool black;
};

#endif
