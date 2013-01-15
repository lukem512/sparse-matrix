#include "ccs.h"

// Creates a CCS node
ccs_node* create_ccs_node () {
    ccs_node* c = malloc (sizeof(ccs_node));

    if (c == NULL) {
        fprintf (stderr, "could not allocate memory for CCS node, out of memory.");
        return c;
    }

    c->val = 0;
    c->row = 0;
    c->next = END_OF_LIST;

    return c;
}

// Frees any memory held by the CCS node
void destroy_ccs_node (ccs_node* c) {
    free (c);
}

// Adds a node to the CCS list for a given values
// Adds to the end of the list
void add_node_to_ccs (ccs* c, unsigned int row, unsigned int val) {
    ccs_node* node = create_ccs_node ();

    if (node == NULL) {
        return;
    }

    // Populate fields
    node->val = val;
    node->row = row;

    // First node added?
    if (c->start == END_OF_LIST) {
        // Populate the start pointer
        c->start = node;
    } else {
        // Update the current end-node's next pointer 
        c->end->next = node;
    }

    // Add node to the end of the list
    c->end = node;

    // Increment counter
    c->cell_count++;
}

// Creates a column structure from a matrix for a given column
ccs* create_ccs (matrix* m, unsigned int id) {
    ccs* c;
    row* r;
    cell* cell;

    // Populate CCS structure
    c = malloc (sizeof(ccs));

    if (c == NULL) {
        fprintf (stderr, "could not allocate memory for CCS structure, out of memory.");
        return c;
    }

    c->col = id;
    c->cell_count = 0;
    c->start = END_OF_LIST;
    c->end = END_OF_LIST;
  
    // Add a cell from each row
    // if one exists for that row
    r = m->start;
    while (r != END_OF_LIST) {
        cell = find_cell_by_id (m, r, id);
        
        if (cell != NULL) {
            add_node_to_ccs (c, r->row, cell->val);
        }

        r = r->next;
    }

    return c;
}

// Frees any memory held by the structure
void destroy_ccs (ccs* c) {
    ccs_node *node, *next;

    // free memory from all nodes
    node = c->start;
    while (node != END_OF_LIST) {
        next = node->next;
        destroy_ccs_node (node);
        node = next;
    }
    
    // ...and from the struct itsself
    free (c);
}

// Prints a CCS to stdout
void print_ccs (ccs* c) {
    ccs_node *n;

    n = c->start;
    while (n != END_OF_LIST) {
        fprintf (stdout, "(%d, %d) %d, ", n->row, c->col, n->val);
        n = n->next;
    }

    fprintf (stdout, "\n");
}
