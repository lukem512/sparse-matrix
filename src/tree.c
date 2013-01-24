// Luke Mitchell <lm0466@my.bristol.ac.uk>
// December 2012
#include "tree.h"

// Creates a tree node with specified value
tree_node* create_tree_node (int val) {
    tree_node* t = malloc (sizeof(tree_node));

    if (t == NULL) {
        fprintf (stderr, "could not allocate space for tree node\n");
        return NULL;
    } 

    t->val = val;
    t->parent = END_OF_LIST;
    t->left = END_OF_LIST;
    t->right = END_OF_LIST;

    return t;
}

// Destroys a tree node
void destroy_tree_node (tree_node* t) {
    free (t);
}

// Destroys a tree from the specified root
void destroy_tree (tree_node* t) {
    if (t != END_OF_LIST) {
        destroy_tree (t->left);
        destroy_tree (t->right);
        destroy_tree_node (t);
    }
}

// Returns 1 if the specified node is a leaf
// i.e. if it has no children
int is_leaf (tree_node* t) {
    if (t->left == END_OF_LIST && t->right == END_OF_LIST) {
        return 1;
    } else {
        return 0;
    }
}

// Returns 1 if the specified node is the root of a tree
int is_root (tree_node* t) {
    if (t->parent == END_OF_LIST) {
        return 1;
    } else {
        return 0;
    }
}
