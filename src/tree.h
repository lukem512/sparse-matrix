// Luke Mitchell <lm0466@my.bristol.ac.uk>
// December 2012

#ifndef __TREE_H
#define __TREE_H

#include "util.h"
#include "matrix.h"

typedef struct tree_node_t {
    int val;
    struct tree_node_t *parent;
    struct tree_node_t *left;
    struct tree_node_t *right;
} tree_node;

tree_node* create_tree_node (int);
void destroy_tree_node (tree_node*);
void destroy_tree (tree_node*);
int is_leaf (tree_node*);
int is_root (tree_node*);

#endif
