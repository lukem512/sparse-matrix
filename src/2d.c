// Luke Mitchell <lm0466@my.bristol.ac.uk>
// December 2012
#include "2d.h"

// Creates a 2D integer array int[x][y]
int** create_2d_int_array (unsigned int x, unsigned int y) {
    int** arr;
    
    // allocate the outer array
    arr= (int**) malloc (x * sizeof(int*));
   
    if (arr == NULL) {
        fprintf (stderr, "could not allocate outer array\n");
        return NULL;
    }

    // allocate each of the inner arrays
    for (int i = 0; i < x; i++) {
        arr[i] = (int*) malloc (y * sizeof(int));

        if (arr[i] == NULL) {
            fprintf (stderr, "could not allocate inner array %d\n", i);
            return NULL;
        }
    }

    // return the pointer
    return arr;
}

// Frees the memory held by a 2D integer array
void destroy_2d_int_array (int** arr, unsigned int x) {
    // free inner array memory
    for (int i = 0; i < x; i++) {
        free (arr[i]);
    }

    // and outer array
    free (arr);
}
