// Luke Mitchell <lm0466@my.bristol.ac.uk>
// December 2012
#include "matrix.h"
#include "ccs.h"
#include "tree.h"
#include "files.h"
#include "2d.h"

// Used for timing
#include <sys/time.h>

#define READ_AND_TRANSPOSE
#define NAIVE_MATRIX_CHAIN_MULTIPLICATION

// TODO
// * memory leaks in stage3
// * stage 5 is SLOW!
// * - to use transpose
// * redo Product to not use 'find' functions

/* Matrix functions */

// Creates a cell with a specified column ID and value
cell* create_cell (unsigned int num, int val) {
    cell* c = malloc(sizeof(cell));
    
    if (c == NULL) {
        return c;
    }

    c->col = num;
    c->val = val;
    c->prev = END_OF_LIST;
    c->next = END_OF_LIST;
    
    return c;
}

// Clone the contents of a cell into another memory location
// Leaving the list pointers as END_OF_LIST
cell* clone_cell (cell* c) {
    cell* cloned = create_cell (c->col, c->val);
    return cloned;
}

// Destroys a cell and frees any held memory
void destroy_cell (cell* c) {
    // Free the memory held by the cell
    free (c);
}

// Creates a row and initialises the values of the struct
row* create_row (unsigned int num) {
    row* r = malloc(sizeof(row));

    if (r == NULL) {
        return r;
    }

    r->row = num;
    r->cell_count = 0;

    r->start = END_OF_LIST;
    r->end = END_OF_LIST;
    
    r->prev = END_OF_LIST;
    r->next = END_OF_LIST;
    
    return r;
}

// Clone the contents of a row into another memory location
// Copies the list of cells into the new row
// Leaving the list pointers as END_OF_LIST
row* clone_row (row* r) {
    row* cloned;
    unsigned int size;

    cloned = create_row(r->row);
    size = sizeof(cell) * r->cell_count;

    // memcpy the linked list of cells
    cloned->start = malloc(size);

    if (cloned->start == NULL) {
        return NULL;
    }

    // TODO - this has an invalid read of size 8
    memcpy (cloned->start, r->start, size);

    // return the cloned structure
    return cloned;
}

// Destroys a row and frees the memory of the cells in it
void destroy_row (row* r) {
    cell *c, *next;

    // Free the memory held by the cells
    c = r->start;
    while (c != END_OF_LIST) {
        next = c->next;
        destroy_cell (c);
        c = next;
    }

    // Free the memory held by the row
    free (r);
}

// Creates a new matrix and initialises the values
matrix* create_matrix (unsigned int rows, unsigned int cols) {
    matrix* mat = malloc (sizeof(matrix));

    if (mat == NULL) {
        return mat;
    }

    mat->cols = cols;
    mat->rows = rows;
    mat->row_count = 0;
    mat->cell_count = 0;

    mat->start = END_OF_LIST;
    mat->end = END_OF_LIST;

    return mat;
}

// Destroys a matrix and frees memory from rows and columns
void destroy_matrix (matrix* m) {
    row *r, *next;

    // Free memory held by the rows
    r = m->start;
    while (r != END_OF_LIST) {
        next = r->next;
        destroy_row (r);
        r = next;
    }

    // Free memory held by matrix
    free (m);
}

// Prints a specified matrix column to stdout
void print_col_by_id (matrix* m, unsigned int id) {
    int i;
    row* r;
    cell* c;
    
    if (m == NULL) {
        return;
    }

    r = m->start;
    i = 0;

    while (i < m->rows) {
        if (r != END_OF_LIST) {
            if (r->row == i) {
                c = find_cell_by_id (m, r, id);
                if (c != NULL) {
                    fprintf (stdout, "%d",c->val);
                } else {
                    fprintf (stdout, "0");
                }
                r = r->next;
            } else {
                fprintf (stdout, "0");
            }
        } else {
            fprintf (stdout, "0");
        }

        if ((i + 1) < m->cols) {
            fprintf (stdout, ",");
        }

        i++;
    }

    fprintf (stdout, "\n");
}

// Prints a specified matrix row to stdout
void print_row_by_id (matrix* m, unsigned int id) {
    row* cur;
   
    if (m == NULL) {
        fprintf (stderr, "specified matrix not defined\n");
        return;
    }

    if (id > m->rows) {
        fprintf (stderr, "specified row not in matrix\n");
        return;
    }

    cur = find_row_by_id (m, id);
    
    if (cur == NULL) {
        print_empty_row (m);
    } else {
        print_row (m, cur);
    }
}

// Prints a matrix row to stdout
void print_row (matrix* m, row* r) {
    unsigned int i;
    cell* cur;
    
    if (m == NULL) {
        fprintf (stderr, "specified matrix not defined\n");
        return;
    }

    cur = r->start;
    i = 0;

    while (i < m->cols) {
        if (cur != END_OF_LIST) {
            if (cur->col == i) {
                fprintf (stdout, "%d",cur->val);
                cur = cur->next;
            } else {
                fprintf (stdout, "0");
            }
        } else {
            fprintf (stdout, "0");
        }

        if ((i + 1) < m->cols) {
            fprintf (stdout, ",");
        }

        i++;
    }

    fprintf (stdout, "\n");
}

// Prints only the non-zero elements in a row
void print_sparse_row (row* r) {
    cell* cur;

    cur = r->start;
    while (cur != END_OF_LIST) {
        fprintf (stdout, "(%d, %d) %d, ", r->row, cur->col, cur->val);
        cur = cur->next;
    }

    fprintf (stdout, "\n");
}

// Prints an empty row of n 0s
void print_empty_row (matrix* m) {
    unsigned int i;
    
    for (i = 0; i < m->cols; i++) {
        fprintf (stdout, "0");

        if ((i + 1) < m->cols) {
            fprintf (stdout, ",");
        }
    }

    fprintf (stdout, "\n");
}

// Prints the value of a cell, specified by its row and column ids
void print_cell_by_id (matrix* m, unsigned int r, unsigned int c) {
    row* row;
    cell* cell;

    row = find_row_by_id (m, r);

    // Empty row?
    if (row == NULL) {
        fprintf (stdout, "0\n");
    } else {
        cell = find_cell_by_id (m, row, c);

        // Empty cell?
        if (cell == NULL) {
            fprintf (stdout, "0\n");
        } else {
            fprintf (stdout, "%d\n", cell->val);
        }
    }

    return;
}

// Prints a specified matrix to stdout
void print_matrix (matrix* m) {
    unsigned int i = 0;
    row* cur;

    if (m == NULL) {
        fprintf (stderr, "specified matrix not defined\n");
        return;
    }

    cur = m->start;

    while (i < m->rows) {
        if (cur->row == i) {
            print_row (m, cur);
            cur = cur->next;
        } else {
            print_empty_row (m);
        }

        i++;
    }
}

void insert_row_into_matrix (matrix* m, row* r) {
    row *cur, *prev;

    cur = m->start;
    prev = NULL;

    // find the previous entry in the list
    if (cur != END_OF_LIST) {
        while (cur->row < r->row) {
            if (cur == m->end) {
                prev = cur;
                cur = END_OF_LIST;
                break;
            }

            prev = cur;
            cur = cur->next;
        }
    }

    // add the new row
    if (prev == NULL) {
        // add at start of list
        r->next = cur;
        m->start = r;

        // add to end of list, if needed
        if (m->end == END_OF_LIST) {
            m->end = r;
        }
    } else {
        // add at prev->next
        r->next = cur;
        r->prev = prev;
        prev->next = r;

        // add to end of list, if needed
        if (prev == m->end) {
            m->end = r;
        } else {
            cur->prev = r;
        }
    }

    // update counters
    m->cell_count += r->cell_count;
    m->row_count++;
}   

void insert_cell_into_row (row* r, cell* c) {
    cell *cur, *prev;

    prev = NULL;
    
    // start search from start or end
    // TODO - currently this does not work, only c->col={0,1} will start at beginning
    if (c->col <= round (r->cell_count/2)) {
        cur = r->start;

        if (cur != END_OF_LIST) {
            while (cur->col < c->col) {
                if (cur == r->end) {
                    prev = cur;
                    cur = END_OF_LIST;
                    break;
                }
                prev = cur;
                cur = cur->next;
            }
        }

        if (prev == NULL) {
            // add at start of list
            c->next = cur;
            r->start = c;

            // add END_OF_LIST pointer to previous
            c->prev = END_OF_LIST;

            // add previous pointer to old start node
            if (cur != END_OF_LIST) {
                cur->prev = c;
            }

            // add to end of list, if needed
            if (r->end == END_OF_LIST) {
                r->end = c;
            }
        } else {
            // add at prev->next
            c->next = cur;
            c->prev = prev;
            prev->next = c;

            // add to end of list, if needed
            if (prev == r->end) {
                r->end = c;
            } else {
                cur->prev = c;
            }
        }
    } else {
        cur = r->end;

        if (cur != END_OF_LIST) {
            while (cur->col > c->col) {
                if (cur == r->start) {
                    prev = cur;
                    cur = END_OF_LIST;
                    break;
                }

                prev = cur;
                cur = cur->prev;
            }
        }

        if (prev == NULL) {
            // Add to start of list?
            if (r->start == END_OF_LIST) {
                r->start = c;
            } else {
                c->prev = cur;
                cur->next = c;
            }
            
            // Add to end of list
            r->end = c;
        } else {
            // Add to start of list?
            if (prev == r->start) {
                r->start = c;
            }

            // Add to end of list?
            if (cur == r->end) {
                cur->next = c;
                r->end = c;
            } else {
                prev->prev = c;
                c->next = prev;
                
                if (cur != END_OF_LIST) {
                    cur->next = c; 
                }
            }

            c->prev = cur;
        }
    }

    // Increment the cell counter
    r->cell_count++;
}

// Finds a row in a matrix with the specified
// id. Returns the row or, if none was found,
// NULL.
row* find_row_by_id (matrix* m, unsigned int id) {
    row* r;

    // begin search from start or end, 
    // depending upon which is closer.
    // this cuts the search time in half
    if (id > round (m->rows/2)) {
        r = m->end;

        while (r != END_OF_LIST) {
            if (r->row == id) {
                return r;
            }
            r = r->prev;
        }
    } else {
        r = m->start;
        
        while (r != END_OF_LIST) {
            if (r->row == id) {
                return r;
            }
            r = r->next;
        }
    }

    // nothing found, return NULL
    return NULL;
}

cell* find_cell_by_id (matrix* m, row* r, unsigned int id) {
    cell* c;

    if (r == NULL) {
        fprintf (stderr, "specified row was NULL\n");
        return NULL;
    }

    if (id > round (m->cols/2)) {
        c = r->end;

        while (c != END_OF_LIST) {
            if (c->col == id) {
                return c;
            }

            c = c->prev;
        }
    } else {
        c = r->start;

        while (c != END_OF_LIST) {
            if (c->col == id) {
                return c;
            }

            c = c->next;
        }
    }

    // nothing found, return NULL
    return NULL;
}

// Performs the matrix transposition operation
// that is, swaps the row and column values
matrix* transpose_matrix (matrix* mat) {
    matrix* newmat;
    row *r, *newr;
    cell *c, *newc;

    // create the transposed matrix structure
    newmat = create_matrix (mat->cols, mat->rows);

    // TODO - QUICK AND DIRTY - TODO REDO
    r = mat->start;
    while (r != END_OF_LIST) {
        c = r->start;
        while (c != END_OF_LIST) {
            newr = find_row_by_id (newmat, c->col);
            
            if (newr == NULL) {
                newr = create_row (c->col);
                insert_row_into_matrix (newmat, newr);
            }

            newc = create_cell (r->row, c->val);
            insert_cell_into_row (newr, newc);

            c = c->next;
        }
        r = r->next;
    }

    return newmat;
}

// Matrix summation
// Add two matrices and output the resulting matrix
matrix* sum_two_matrices (matrix* m1, matrix* m2) {
    matrix* summed;
    row *r1, *r2;
    cell *c1, *c2;

    // check matrices are not NULL
    if (m1 == NULL || m2 == NULL) {
        fprintf (stderr, "one or more specified matrices are NULL\n");
        return NULL;
    }

    // ensure dimensions are the same
    // in the assignment document this says STDOUT but I believe
    // it should be STDERR
    if (m1->rows != m2->rows || m1->cols != m2->cols) {
        fprintf (stderr, "matrix dimensions do not match, aborting\n");
        return NULL;
    }

    // create new matrix to hold result
    summed = create_matrix (m1->rows, m1->cols);

    if (summed == NULL) {
        fprintf (stderr, "could not create matrix\n");
        return NULL;
    }

    // add each cell in m1 with the corresponding cell in m2
    r1 = m1->start;
    r2 = m2->start;

    while ((r1 != END_OF_LIST) && (r2 != END_OF_LIST)) {
        if (r1->row <= r2->row) {
            if (r1->row == r2->row) {
                row *r = create_row (r1->row);
            
                c1 = r1->start;
                c2 = r2->start;
            
                cell* c;

                while ((c1 != END_OF_LIST) && (c2 != END_OF_LIST)) {
                    if (c1->col <= c2->col) {
                        if (c1->col == c2->col) {
                            // Create a cell using the summed values
                            c = create_cell (c1->col, c1->val + c2->val);

                            // Advance both pointers
                            c1 = c1->next;
                            c2 = c2->next;
                        } else {
                            // Create a cell using the c1 value
                            c = create_cell (c1->col, c1->val);

                            // Advance c1 pointer
                            c1 = c1->next;
                        }
                    } else {
                        // Create a cell using the c2 value
                        c = create_cell (c2->col, c2->val);
                        
                        // Advance c2 pointer
                        c2 = c2->next;
                    }

                    // Add the new cell to the row
                    // Only if the value is non-zero
                    if (c->val != 0) {
                        insert_cell_into_row (r, c);
                    } else {
                        destroy_cell (c);
                    }
                }

                while (c1 != END_OF_LIST) {
                    // Create a cell using the c1 value
                    c = create_cell (c1->col, c1->val);
                    
                    // Advance c1 pointer
                    c1 = c1->next;

                    // Insert new cell into the row
                    insert_cell_into_row (r, c);
                }
            
                while (c2 != END_OF_LIST) {
                    // Create a cell using the c2 value
                    c = create_cell (c2->col, c2->val);
                    
                    // Advance c2 pointer
                    c2 = c2->next;

                    // Insert new cell into the row
                    insert_cell_into_row (r, c);
                }

                // Insert the row into the matrix
                insert_row_into_matrix (summed, r);

                // Advance both row pointers
                r1 = r1->next;
                r2 = r2->next;
            } else {
                // r1->row < r2->row
                // Add the whole of r1
	        row* r = clone_row (r1);
                insert_row_into_matrix (summed, r);
                r1 = r1->next;
            }
        } else {
            // r1->row > r2->row
            // Add the whole of r2
	    row* r = clone_row (r2);
            insert_row_into_matrix (summed, r);
            r2 = r2->next;
        }
    }

    while (r1 != END_OF_LIST) {
        // Add whole row from r1
	row* r = clone_row (r1);
        insert_row_into_matrix (summed, r);
        r1 = r1->next;
    }

    while (r2 != END_OF_LIST) {
        // Add whole row from r2
	row* r = clone_row (r2);
        insert_row_into_matrix (summed, r);
        r2 = r2->next;
    }
    
    // return the new matrix
    return summed;
}

// Matrix product
// Computes the product of a matrix m and a transposed matrix t
// As t is transposed, the rows can be used as columns
// TODO - test me!
matrix* product_two_matrices_transposed (matrix *m, matrix *t) {
    matrix* prod;
    row *row, *col, *prod_row;
    cell *row_cell, *col_cell, *prod_cell;
    unsigned int total;

    // ensure the product is defined for m and t
    if (m->cols != t->cols) {
        fprintf (stderr, "matrix product not defined for inputs, aborting\n");
        return NULL;
    }
    
    // allocate some space for the new matrix
    prod = create_matrix (m->rows, t->rows);

    if (prod == NULL) {
        fprintf (stderr, "could not create matrix\n");
        return NULL;
    }

    printf ("PRODUCT_TWO_MATRICES_TRANSPOSED: begin...\n");

    // initialise the row pointer
    row = m->start;
    prod_row = prod->start;

    // step through the matrices and calculate cells
    while (row != END_OF_LIST) {
        // initialise the col pointer
        col = t->start;

        //printf ("Row %d\n", row->row);

        // create a new row
        prod_row = create_row (row->row);

        while (col != END_OF_LIST) {
            // compute (row->row, col->row)
            row_cell = row->start;
            col_cell = col->start;

      //      printf ("Col %d\n", col->row);

            // set total to 0
            total = 0;

            // step through row and column
            while ((row_cell != END_OF_LIST) && (col_cell != END_OF_LIST)) {
                if (row_cell->col == col_cell->col) {
                    // add to total
                    total += row_cell->val * col_cell->val;

    //                printf ("Total is %d\n", total);
    
                    col_cell = col_cell->next;
                    row_cell = row_cell->next;
                } else if (row_cell->col > col_cell->col) {
                    col_cell = col_cell->next;
                } else {
                    row_cell = row_cell->next;
                }
            }
        
            // add cell to new matrix
            if (total != 0) {
                // create the cell
                prod_cell = create_cell (col->row, total);

                // add the cell to the end of the row
                if (prod_row->end != END_OF_LIST) {
                    prod_row->end->next = prod_cell;
                    prod_cell->prev = prod_row->end;
                }
                
                prod_row->end = prod_cell;

                // add to start of row?
                if (prod_row->start == END_OF_LIST) {
                    prod_row->start = prod_cell;
                }

                // increment cell counter
                prod_row->cell_count++;
            }

            // increment column
            col = col->next;
        }

        if (prod_row->cell_count > 0) {
            // add the row to the end of the matrix
            if (prod->end != END_OF_LIST) {
                prod->end->next = prod_row;
                prod_row->prev = prod->end;
            }

            prod->end = prod_row;

            // add to start of matrix?
            if (prod->start == END_OF_LIST) {
                prod->start = prod_row;
            }

            // increment the counters
            prod->row_count++;
            prod->cell_count += prod_row->cell_count;
        } else {
            destroy_row (prod_row);
        }

        // increment row
        row = row->next;   
    }

    // return product
    return prod;
}

// Matrix product
// Computes the product of matrices m1 and m2
matrix* product_two_matrices (matrix* m1, matrix* m2) {
    matrix* prod;
    row *row, *prod_row;
    cell *row_cell, *prod_cell;
    ccs* col;
    ccs_node* col_cell;
    unsigned int total;

    // ensure the product is defined for m1 and m2
    if (m1->cols != m2->rows) {
        fprintf (stderr, "matrix product not defined for inputs, aborting\n");
        return NULL;
    }

    // Allocate some space for the new matrix
    prod = create_matrix (m1->rows, m2->cols);

    if (prod == NULL) {
        fprintf (stderr, "could not create matrix\n");
        return NULL;
    }

    // Perform all calculations requiring first column first,
    // then second, etc.
    // Everything in column i requires CCS for column i from m2
    for (int i = 0; i < m2->cols; i++) {
        // Create a struct for the column
        col = create_ccs (m2, i);
        
        if (col->cell_count == 0) {
            destroy_ccs (col);
            continue;
        }

        // Compute values for every row in column
        row = m1->start;
        prod_row = prod->start;
        while (row != END_OF_LIST) {
            // Multiply column values by the row values
            total = 0;
            col_cell = col->start;
            row_cell = row->start;

            while ((col_cell != END_OF_LIST) && (row_cell != END_OF_LIST)) {
                if (col_cell->row <= row_cell->col) {
                    if (col_cell->row == row_cell->col) {
                        // Corresponding cells found, compute product
                        total += (col_cell->val * row_cell->val);

                        // Increment list pointers
                        col_cell = col_cell->next;
                        row_cell = row_cell->next;
                    } else {
                        // col_cell->row < row_cell->col
                        col_cell = col_cell->next;
                    }
                } else {
                    // col_cell->row > row_cell->coll
                    row_cell = row_cell->next;
                }
            }
            
            // if the cell is non-zero, add it to the new matrix
            if (total != 0) {
                // does this row exist in prod yet?
                if (prod_row == END_OF_LIST) {
                    // create it
                    prod_row = create_row (row->row);

                    // insert it into matrix
                    if (prod->start == END_OF_LIST) {
                        prod->start = prod_row;
                        prod->end = prod_row;
                    } else {
                        prod->end->next = prod_row;
                        prod_row->prev = prod->end;
                        prod->end = prod_row;
                    }
                } else if (prod_row->row != row->row) {
                    // TODO - sort this out, this is very slow!
                    prod_row = find_row_by_id (prod, row->row);

                    if (prod_row == NULL) {
                        prod_row = create_row (row->row);
                        insert_row_into_matrix (prod, prod_row);
                    }
                }

                // create the cell with value 'total'
                prod_cell = create_cell (i, total);

                if (prod_row->start == END_OF_LIST) {
                    // add it to the row
                    prod_row->start = prod_cell;
                    prod_row->end = prod_cell;
                } else {
                    // add it to the end of the row
                    prod_cell->prev = prod_row->end;
                    prod_row->end->next = prod_cell;
                    prod_row->end = prod_cell;
                }
               
                // increment cell counters
                prod_row->cell_count++;
                prod->cell_count++;

                // increment product matrix row pointer
                prod_row = prod_row->next;
            }
    
            // increment row pointer
            row = row->next;
        }

        // free memory
        destroy_ccs (col);
    }

    // return the product matrix
    return prod;
}

// Used to extract the sequence with which to multiply
// the matrix chain by. This returns a binary tree.
tree_node* create_tree_from_seq(int** seq, int i, int j, tree_node* parent) {
    tree_node* t;
    int k;
    
    // Allocate some memory
    t = create_tree_node (-1);

    if (t == NULL)
        return NULL;

    // Add parent
    t->parent = parent;
   
    // Recursively create subtrees
    if (i == j) {
        t->val = i;
    } else {
        k = seq[i][j];
        t->left = create_tree_from_seq (seq, i, k, t);
        t->right = create_tree_from_seq (seq, k+1, j, t);
    }

    // Return the tree node!
    return t;
}

// Returns an array with the indices of the matrices in
// the optimal order for multiplication.
// Algorithm taken from en.wikipedia.org/wiki/Matrix_chain_multiplication
// Runs in O(n^3) time.
tree_node* optimal_matrix_chain (int p[], int n) {
    int **ops, **seq;
    tree_node* t;

    // Allocate 2 2D arrays
    ops = create_2d_int_array (n, n);
    seq = create_2d_int_array (n, n);

    if (ops == NULL || seq == NULL) {
        return NULL;
    }

    // Calculate the optimal sequence
    for (int l = 0; l < n; l++) {
        for (int i = 0; i < n - l; i++) {
            int j = i + l;
            ops[i][j] = -1;
            seq[i][j] = -1;
            
            for (int k = i; k < j; k++) {
                int q = ops[i][k] + ops[k+1][j] + p[i]*p[k+1]*p[j+1];
                if (ops[i][j] == -1 || q < ops[i][j]) {
                    ops[i][j] = q;
                    seq[i][j] = k;
                }
            }
        }
    }

    // construct tree from seq
    t = create_tree_from_seq(seq, 0, n-1, END_OF_LIST);

    // free arrays
    destroy_2d_int_array (ops, n);
    destroy_2d_int_array (seq, n);

    // return the tree
    return t;
}

// Returns the product of a list of matrices
// Computed in a naive manner
matrix* calculate_naive_matrix_chain (char *X_name[], int l) {
    matrix *prod, *m1, *m2;

    // read first matrix from file
    prod = read_matrix_from_file (X_name[0]);

    if (prod == NULL) {
	fprintf (stderr, "could not allocate memory for matrix, aborting.");
        return NULL;
    }


    for (int i = 1; i < l; i++) {
	// read the next matrix from its file
#ifdef READ_AND_TRANSPOSE
        m1 = read_and_transpose_matrix_from_file (X_name[i]);
#else
        m1 = read_matrix_from_file (X_name[i]);
#endif

        if (m1 == NULL) {
	    fprintf (stderr, "could not allocate memory for matrix, aborting.");
            destroy_matrix (prod);
            return NULL;
        }

	// store a pointer to the old product
        m2 = prod;

	// calculate the new product
#ifdef READ_AND_TRANSPOSE
        prod = product_two_matrices_transposed (prod, m1);
#else
        prod = product_two_matrices (prod, m1);
#endif

	// free unused memory
        destroy_matrix (m1);
        destroy_matrix (m2);
    }    

    // return the product!
    return prod;
}

// Calculates the product of a tree using DFS
// TODO - use transpose product
matrix* calculate_product_of_tree (tree_node* t, char* X_name[]) {
    matrix *l, *r, *prod;

    if (is_leaf(t)) {
        // node is a leaf, read the matrix from its file
        prod = read_matrix_from_file (X_name[t->val]);
    } else {
        // recursively get values for branches
        l = calculate_product_of_tree (t->left, X_name);
        r = calculate_product_of_tree (t->right, X_name);

        // compute the product of branches
        prod = product_two_matrices (l, r);

        // clean up memory
        destroy_matrix (l);
        destroy_matrix (r);
    }

    return prod;
}

/* Stage functions */

/*
Perform stage 1:

- given a matrix X  stored in a file  named by X_name,
- inspect X, i.e., print the element, row and column at row r, column c.
*/
void stage1( char* X_name, int r, int c ) {
    // read the matrix from specified filename
    matrix* mat = read_matrix_from_file (X_name);

    // check for errors
    if (mat == NULL) {
        fprintf (stderr, "could not read one or more input matrices\n");
        return;
    }

    // output value of element
    print_cell_by_id (mat, r, c);

    // output CSV values of row
    print_row_by_id (mat, r);

    // output CSV values of column
    print_col_by_id (mat, c);

    // free memory
    destroy_matrix (mat);
}

/*
Perform stage 2:

- given a matrix X         stored   in a file  named by X_name,
- compute R = X^T, i.e., the transpose of X, 
- then                     store  R in a file  named by R_name.
*/

void stage2( char* R_name, char* X_name ) {
    matrix *mat, *tran;

    // TODO - remove debugging
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    // read the matrix from specified filename
#ifdef READ_AND_TRANSPOSE
    mat = read_and_transpose_matrix_from_file (X_name);
#else
    mat = read_matrix_from_file (X_name);
#endif

    // check for errors
    if (mat == NULL) {
        fprintf (stderr, "could not read input matrix\n");
        return;
    }

    // transpose
#ifdef READ_AND_TRANSPOSE
    tran = mat;
#else
    tran = transpose_matrix (mat);
#endif

    // output to file R_name
    write_matrix_to_file (tran, R_name);

    // free memory
#ifndef READ_AND_TRANSPOSE
    destroy_matrix (mat);
#endif
    destroy_matrix (tran);
    
    // TODO - remove debugging
    gettimeofday(&tv2, NULL); 
    printf ("Stage2 took %f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec));
}

/*
Perform stage 3:

- given   matrices X and Y stored   in   files named by X_name and Y_name,
- compute R = X + Y, i.e., the sum of X and Y,
- then                     store  R in a file  named by R_name.
*/

void stage3( char* R_name, char* X_name, char* Y_name ) {
    matrix *m1, *m2, *sum;
    
    // TODO - remove debugging
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    
    // read the input matrices from specified filenames
    m1 = read_matrix_from_file (X_name);
    m2 = read_matrix_from_file (Y_name);

    // check for errors
    if (m1 == NULL || m2 == NULL) {
        fprintf (stderr, "could not read one or more input matrices\n");
        return;
    }

    // sum the matrices
    sum = sum_two_matrices (m1, m2);

    // output to file R_name
    write_matrix_to_file (sum, R_name);

    // free memory
    destroy_matrix (m1);
    destroy_matrix (m2);
    destroy_matrix (sum); // TODO - this has invalid destroy_cell call
    
    // TODO - remove debugging
    gettimeofday(&tv2, NULL); 
    printf ("Stage3 took %f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec));
}

/*
Perform stage 4:

- given   matrices X and Y stored   in   files named by X_name and Y_name,
- compute R = X \cdot Y, i.e., the product of x and y,
- then                     store  R in a file  named by R_name.
*/

void stage4( char* R_name, char* X_name, char* Y_name ) {
    matrix *m1, *m2, *prod;
    
    // TODO - remove debugging
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    
    // read the input matrices from specified filenames
    m1 = read_matrix_from_file (X_name);
    m2 = read_matrix_from_file (Y_name);
    
    // check for errors
    if (m1 == NULL || m2 == NULL) {
        fprintf (stderr, "could not read one or more input matrices\n");
        return;
    }

    // calculate the product of the two matrices
    prod = product_two_matrices (m1, m2);

    // write result to file
    write_matrix_to_file (prod, R_name);

    // free memory
    destroy_matrix (m1);
    destroy_matrix (m2);
    destroy_matrix (prod);

    // TODO - remove debugging
    gettimeofday(&tv2, NULL); 
    printf ("Stage4 took %f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec));
}

/*
Perform stage 5:

- given l matrices X_i     stored in     files named by each X_name[ i ],
- compute R = \prod_{i=0}^{i < l} X_i, i.e., the product of the matrices,
- then                     store  R in a file  named by R_name.
*/
void stage5( char* R_name, char* X_name[], int l ) {
    matrix *m;    

    // TODO - remove debugging
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

#ifdef NAIVE_MATRIX_CHAIN_MULTIPLICATION
    // calculate the product
    m = calculate_naive_matrix_chain (X_name, l);
#else
    tree_node* t;
    int* p;
    
    // create an array of dimensions of the matrices
    p = malloc ((l+1) * sizeof(int));
    
    if (p == NULL) {
        fprintf (stderr, "could not allocate memory for dimension list\n");
        return;
    }

    for (int i = 0; i < l; i++) {
        // read matrix dimensions from file
        m = read_matrix_dimensions_from_file (X_name[i]);
        
        // check it for errors
        if (m == NULL) {
            fprintf (stderr, "could not read input matrix dimensions %s\n", X_name[i]);
            return;
        }

        // add the dimension(s) to the array
        if (i == 0) {
            p[i] = m->rows;
        }

        p[i+1] = m->cols;

        // free memory
        destroy_matrix (m);
    }

    // calculate a tree containing the optimal order
    // for multiplying the matrices
    t = optimal_matrix_chain (p, l);

    // compute the product by performing a depth-first search
    // on the tree
    m = calculate_product_of_tree (t, X_name);

    // free memory
    destroy_tree (t);
    free (p);
#endif

    // write result to file
    write_matrix_to_file (m, R_name);

    // free memory
    destroy_matrix (m);
    
    // TODO - remove, debugging
    gettimeofday(&tv2, NULL); 
    printf ("Stage5 took %f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec));
}

/*
The main function acts as a driver for the assignment by simply invoking
the correct function for the requested stage.
*/

int main( int argc, char* argv[] ) {
    if     ( !strcmp( argv[ 1 ], "stage1" ) ) {
        stage1( argv[ 2 ], atoi( argv[ 3 ] ), atoi( argv[ 4 ] ) );
    }
    else if( !strcmp( argv[ 1 ], "stage2" ) ) {
        stage2( argv[ 2 ], argv[ 3 ] );
    }
    else if( !strcmp( argv[ 1 ], "stage3" ) ) {
        stage3( argv[ 2 ], argv[ 3 ], argv[ 4 ] );
    }
    else if( !strcmp( argv[ 1 ], "stage4" ) ) {
        stage4( argv[ 2 ], argv[ 3 ], argv[ 4 ] );
    }
    else if( !strcmp( argv[ 1 ], "stage5" ) ) {
        stage5( argv[ 2 ], argv + 3, argc - 3 );
    }

    return 0;
}
