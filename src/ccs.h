#ifndef __CCS_H
#define __CCS_H

#include "util.h"
#include "matrix.h"

typedef struct ccs_node_t {
    unsigned int val;
    unsigned int row;
    struct ccs_node_t*  next;
} ccs_node;

typedef struct {
    unsigned int col;
    unsigned int cell_count;
    ccs_node* start;
    ccs_node* end;
} ccs;

ccs_node* create_ccs_node ();
void destroy_ccs_node (ccs_node* c);

ccs* create_ccs (matrix* m, unsigned int id);
void destroy_ccs (ccs* c);

void add_node_to_ccs (ccs* c, unsigned int row, unsigned int val);

void print_ccs (ccs* c);

#endif
