#ifndef __FILES_H
#define __FILES_H

#include "util.h"
#include "matrix.h"

matrix* read_matrix_dimensions_from_file (char* filename);
matrix* read_matrix_from_file (char* filename);
matrix* do_read_matrix_from_file (char* filename, int dimensions);
void write_matrix_to_file (matrix* mat, char* filename);

#endif
