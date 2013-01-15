// Luke Mitchell <lm0466@my.bristol.ac.uk>
// December 2012
#include "files.h"

/* Helper functions */

// Adds a letter, c, to a string, str
char* add_letter_to_string (char* str, char c) {
	int len = 0;
	char *buf = NULL;

	// empty string?
	if (str != NULL) {
		// no, it's not
		// copy str to buf
		len = strlen (str) + 1;
		buf = calloc (len, sizeof (char));
		strncpy (buf, str, len);

		// resize buf
		free (str);
		str = calloc (len + 1, sizeof (char));
					
		// copy buffer to str
		strncpy (str, buf, len);

		// add the letter
		strncpy (&str[len-1], &c, 1);

                // and the terminating byte
                str[len] = '\0';

		// free buf
		free (buf);
	} else {
		// yes
		// allocate a buffer	
		str = malloc (2 * sizeof (char));
					
		// add the letter
		strncpy (str, &c, 1);

                // add the terminating byte
                str[1] = '\0';
	}

	return str;
}

/* File IO functions */

// Reads the dimensions of a matrix from a file in the specified format
matrix* read_matrix_dimensions_from_file (char* filename) {
    return do_read_matrix_from_file (filename, 1);
}

// Reads a matrix from a file in the specified format
matrix* read_matrix_from_file (char* filename) {
    return do_read_matrix_from_file (filename, 0);
}

// Helper function: returns the matrix or just the dimensions
// depending upon the value of the dimensions paramter (!0 for just dimensions)
matrix* do_read_matrix_from_file (char* filename, int dimensions) {
    unsigned int rows, cols;
    int end_of_file;
    char* buf;
    int w, i, line, first_line;
    int rid, cid, val;
    FILE* fp;
    
    matrix* mat;
    row* row;
    cell* cell;

    // initialise variables
    mat = NULL;
    rows = cols = 0;
    rid = cid = 0;

    // read in file X_name
    fp = fopen (filename, "r");

    if (fp == NULL) {
        fprintf (stderr, "could not open %s\n", filename);
        return NULL;
    }

    // set flags before entering loop
    end_of_file = 0;
    first_line = 1;
    line = 1;

    while (!end_of_file) {
        buf = NULL;
        i= 0;

	while (1) {
	    // get character
	    w = fgetc (fp);

            // newline?
            if (w == '\n') {
                //buf = add_letter_to_string (buf, '\0');

                if (first_line) {
                    // unset first_line flag
                    first_line = 0;

                    // extract the number of columns
                    cols = atoi (buf);
                    
                    // create the matrix data structure
                    mat = create_matrix (rows, cols);
                    if (mat == NULL) {
                        fprintf (stderr, "could not create matrix\n");
                        fclose (fp);
                        return NULL;
                    }
    
                    // free buffer memory
                    free (buf);
                    buf = NULL;

                    // return just the dimensions if req'd
                    if (dimensions) {
                        return mat;
                    }
                } else {
                    val = atoi (buf);
                    
                    // create cell and row (if necessary)
                    row = find_row_by_id (mat, rid);
                    if (row == NULL) {
                        row = create_row (rid);
                        insert_row_into_matrix (mat, row);
                    }
    
                    // add the cell
                    // the commented code is very slow and unneccesary
                    //cell = find_cell_by_id (mat, row, cid);
                    //if (cell == NULL) {
                        cell = create_cell (cid, val);
                        insert_cell_into_row (row, cell);
                    //} else {
                    //    fprintf (stderr, "a value has already been specified for (%d,%d)\n",rid,cid);
                    //    fclose (fp);
                    //    return NULL;
                    //}

                    // free buffer memory
                    free (buf);
                    buf = NULL;
                }
                break;
            }

            // break if the end of file is reached
	    if (w == EOF) {
	        end_of_file = 1;
		break;
            }

            // comma signifies the end of a value
	    if (w == ',') {
                //buf = add_letter_to_string (buf, '\0');

                switch (i) {
                    // row
                    case 0:
                        if (first_line) {
                            rows = atoi(buf);
                        } else {
                            rid = atoi (buf);
                        }
                        free (buf);
                        buf = NULL;
                    break;
                    
                    // column
                    case 1:
                        if (first_line) {
                            fprintf (stderr, "line %d not in correct format\n", line);
                            fclose (fp);
                            return NULL;
                        } else {
                            cid = atoi (buf);
                        }
                        free (buf);
                        buf = NULL;
                    break;

                    default:
                        fprintf (stderr, "line %d not in correct format\n", line);
                        fclose (fp);
                        return NULL;
                }

                i++;
                continue;
            }

	    // add digit to the buffer
            if ((w >= '0' && w <= '9') || (w == '-')) {
	        buf = add_letter_to_string (buf, w);
            } else {
                fprintf (stderr, "read invalid character from file, ignoring\n");
            }
	}

        line++;
    }

    // close the file
    fclose (fp);

    return mat;
}

// Writes a matrix to a file in the specified format
void write_matrix_to_file (matrix* mat, char* filename) {
    FILE* fp;
    char buffer[64];
    row* r;
    cell* c;

    // ensure matrix is not null
    if (mat == NULL) {
        fprintf (stderr, "specified matrix was NULL\n");
        return;
    }
    
    // open the file
    fp = fopen (filename, "w");
    
    if (fp == NULL) {
        fprintf (stderr, "could not open %s\n", filename);
        return;
    }

    // write dimensions as first line
    snprintf(buffer, 64, "%d,%d\n", mat->rows, mat->cols);
    fputs (buffer, fp);

    // write non-zero cell values
    r = mat->start;
    while (r != END_OF_LIST) {
        c = r->start;
        while (c != END_OF_LIST) {
            // write cell (row,col,val)
            snprintf(buffer, 64, "%d,%d,%d\n", r->row, c->col, c->val);
            fputs (buffer, fp);
            c = c->next;
        }
        r = r->next;
    }

    // close the file
    fclose (fp);
}
