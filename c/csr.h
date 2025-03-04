#ifndef CSR_H
#define CSR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "memoryarena.h"

// CSR matrix structure
typedef struct
{
    int n_row;
    int n_col;
    int *indices; // Column indices of nonzero values
    int *indptr;  // Row pointer array
    double *data; // Nonzero values
    int nnz_cout; // Current count of nonzero elements
    int nnz_max;  // Maximum allowed nonzero elements
} CSR;

// Functions for managing CSR matrices
void init_csr(MemoryArena *arena, CSR *csr, int n_row, int n_col, int nnz_max);
void insert_val_csr(CSR *csr, int row, int col, double val);
void print_csr(CSR *csr);
void print_dense_csr(CSR *csr);

#endif // CSR_H