#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <stddef.h>

int radix_sort(const int *arr, size_t n, int *result_array);

int radix_sort_char(const char**arr, size_t n, const char**result_array);



int radix_sort_r(const int *arr, size_t n, int *result_array, int radix);

#endif
