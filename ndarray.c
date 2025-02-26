#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  double *data;
  int ndim;
  int *shape;
  int *strides;
  int *ref_count;
} ndarray;

int calculate_size(int ndim, int *shape)
{
  int size = 1;
  for (int i = 0; i < ndim; i++)
  {
    size *= shape[i];
  }
  return size;
}

int *calculate_strides(int ndim, int *shape)
{
  int *strides = (int *)malloc(sizeof(int) * ndim);
  strides[ndim - 1] = 1;
  for (int i = ndim - 2; i >= 0; i--)
  {
    strides[i] = strides[i + 1] * shape[i + 1];
  }
  return strides;
}

ndarray *ndarray_new(int ndim, ...)
{
  va_list args;
  va_start(args, ndim);

  // Allocate the shape array and fill it
  int *shape = (int *)malloc(sizeof(int) * ndim);
  for (int i = 0; i < ndim; i++)
  {
    shape[i] = va_arg(args, int);
  }

  va_end(args);

  // Compute the total number of elementes
  int size = calculate_size(ndim, shape);

  // Allocate data and init strides
  double *data = (double *)malloc(sizeof(double) * size);
  int *strides = calculate_strides(ndim, shape);

  // Create the ndarray object and fill it
  ndarray *nd = (ndarray *)malloc(sizeof(ndarray));
  nd->data = data;
  nd->ndim = ndim;
  nd->shape = shape;
  nd->strides = strides;

  // Allocate reference counter
  nd->ref_count = (int *)malloc(sizeof(int));
  *nd->ref_count = 1;

  return nd;
}

void ndarray_free(ndarray *nd)
{
  if (nd == NULL)
    return;
  (*nd->ref_count)--;
  if (*nd->ref_count == 0)
  {
    free(nd->data);
    free(nd->ref_count);
  }
  free(nd->shape);
  free(nd->strides);
  free(nd);
}

int get_linear_index(ndarray *nd, int *indices)
{
  int index = 0;
  for (int i = 0; i < nd->ndim; i++)
  {
    index += indices[i] * nd->strides[i];
  }
  return index;
}

double ndarray_get(ndarray *nd, int *indices)
{
  int index = get_linear_index(nd, indices);
  return nd->data[index];
}

void ndarray_set(ndarray *nd, int *indices, double value)
{
  int index = get_linear_index(nd, indices);
  nd->data[index] = value;
}

ndarray *ndarray_slice(ndarray *nd, int *start, int *end, int *steps)
{
  int *new_shape = (int *)malloc(sizeof(int) * nd->ndim);
  int *new_strides = (int *)malloc(sizeof(int) * nd->ndim);
  int total_elements = 1;

  for (int i = 0; i < nd->ndim; i++)
  {
    new_shape[i] = (end[i] - start[i]) / steps[i];
    new_strides[i] = nd->strides[i] * steps[i];
    total_elements *= new_shape[i];
  }

  ndarray *slice = (ndarray *)malloc(sizeof(ndarray));
  slice->data = nd->data;
  slice->ndim = nd->ndim;
  slice->shape = new_shape;
  slice->strides = new_strides;

  // Update the reference count
  slice->ref_count = nd->ref_count;
  (*slice->ref_count)++;

  return slice;
}

void ndarray_print(ndarray *nd)
{
  int *indices = (int *)malloc(sizeof(int) * nd->ndim);
  for (int i = 0; i < nd->shape[0]; i++)
  {
    for (int j = 0; j < nd->shape[1]; j++)
    {
      indices[0] = i;
      indices[1] = j;
      printf("%.3f ", ndarray_get(nd, indices));
    }
    printf("\n");
  }
}

void ndarray_print_strides(ndarray *nd)
{
  for (int i = 0; i < nd->ndim; i++)
  {
    printf("%d ", nd->strides[i]);
  }
  printf("\n");
}

int main(void)
{
  ndarray *nd = ndarray_new(2, 3, 4);

  // Set some values
  int indices[2] = {0, 0};
  ndarray_set(nd, indices, 1.0);
  indices[0] = 2;
  indices[1] = 3;
  ndarray_set(nd, indices, 2.0);

  // Print the array
  ndarray_print(nd);

  ndarray_free(nd);

  return 0;
}