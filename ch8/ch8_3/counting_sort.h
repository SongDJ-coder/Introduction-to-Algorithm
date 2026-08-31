#ifndef COUNTING_SORT_H
#define COUNTING_SORT_H

#include <stddef.h>

int counting_sort(const int *arr, size_t n, int *result_array, int k);

int counting_sort_v2(const int *arr, size_t n, int *result_array, int k, int div);

#endif
