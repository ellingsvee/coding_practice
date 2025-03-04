#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "memoryarena.h"
#include "csr.h"

void init_csr(MemoryArena *arena, CSR *csr, int n_row, int n_col, int nnz_max)
{
    csr->n_row = n_row;
    csr->n_col = n_col;
    csr->nnz_cout = 0;
    csr->nnz_max = nnz_max;

    csr->indices = (int *)arena_alloc(arena, nnz_max * sizeof(int));
    csr->indptr = (int *)arena_alloc(arena, (n_row + 1) * sizeof(int));
    csr->data = (double *)arena_alloc(arena, nnz_max * sizeof(double));

    for (int i = 0; i < n_row + 1; i++)
    {
        csr->indptr[i] = 0;
    }
}

void insert_val_csr(CSR *csr, int row, int col, double val)
{
    if (csr->nnz_cout + 1 > csr->nnz_max)
    {
        fprintf(stderr, "CSR matrix storage is full!\n");
        exit(EXIT_FAILURE);
    }

    int pos = csr->nnz_cout;
    csr->data[pos] = val;
    csr->indices[pos] = col;

    for (int i = row + 1; i < csr->n_row + 1; i++)
    {
        csr->indptr[i]++;
    }

    csr->nnz_cout++;
}

void print_csr(CSR *csr)
{
    printf("indptr: ");
    for (int i = 0; i < csr->n_row + 1; i++)
    {
        printf("%d ", csr->indptr[i]);
    }
    printf("\nindices: ");
    for (int i = 0; i < csr->nnz_cout; i++)
    {
        printf("%d ", csr->indices[i]);
    }
    printf("\ndata: ");
    for (int i = 0; i < csr->nnz_cout; i++)
    {
        printf("%.2f ", csr->data[i]);
    }
    printf("\n");
}

void print_dense_csr(CSR *csr)
{
    for (int row = 0; row < csr->n_row; row++)
    {
        int start = csr->indptr[row];   // Start index for this row
        int end = csr->indptr[row + 1]; // End index for this row
        int nnz_idx = start;            // Position in indices/data arrays

        for (int col = 0; col < csr->n_col; col++)
        {
            if (nnz_idx < end && csr->indices[nnz_idx] == col)
            {
                printf("%6.2f ", csr->data[nnz_idx]); // Print nonzero value
                nnz_idx++;                            // Move to the next nonzero entry
            }
            else
            {
                printf("  0.00 "); // Print zero for missing elements
            }
        }
        printf("\n"); // Move to the next row
    }
}

int main(void)
{
    MemoryArena arena;
    init_arena(&arena, ARENA_SIZE);

    CSR csr;
    init_csr(&arena, &csr, 3, 3, 10);
    insert_val_csr(&csr, 0, 0, 3.5);
    insert_val_csr(&csr, 1, 1, 4.2);
    insert_val_csr(&csr, 2, 2, -1.0);

    // Print
    print_csr(&csr);
    print_dense_csr(&csr);

    // Free the arena
    free_arena(&arena);

    return 0;
}