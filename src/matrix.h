#ifndef __MATRIX_H
#define __MATRIX_H

#include "util.h"

#define INT32_LENGTH 10
#define UINT32_LENGTH 10
#define END_OF_LIST (void*) -1

/* Data type definitions */

typedef struct cell_t {
    unsigned int col;
    int val;

    struct cell_t* prev;
    struct cell_t* next;
} cell;

typedef struct row_t {
    unsigned int row;
    unsigned int cell_count;

    cell* start;
    cell* end;

    struct row_t* prev;
    struct row_t* next;
} row;

typedef struct matrix_t {
    unsigned int rows;
    unsigned int cols;
    unsigned int row_count;
    unsigned int cell_count;

    row* start;
    row* end;
} matrix;

/* Function prototypes */

cell* create_cell (unsigned int num, int val);
row* create_row (unsigned int num);
matrix* create_matrix (unsigned int rows, unsigned int cols);

void insert_row_into_matrix (matrix* m, row* r);
void insert_cell_into_row (row* r, cell* c);

row* find_row_by_id (matrix* m, unsigned int id);
cell* find_cell_by_id (matrix* m, row* r, unsigned int id);

void print_row (matrix* m, row* r);
void print_empty_row (matrix* m);
void print_row_by_id (matrix* m, unsigned int id);
void print_col_by_id (matrix* m, unsigned int id);
void print_cell_by_id (matrix* m, unsigned int r, unsigned int c);
void print_matrix (matrix* m);

matrix* transpose_matrix (matrix* mat);
matrix* sum_two_matrices (matrix* m1, matrix* m2);
#endif
