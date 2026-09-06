#ifndef MINMAX_H
#define MINMAX_H
#include <stddef.h>

extern size_t g_comparisons;

int min_max(const int *arr, size_t n, int *min, int *max);   // 0 성공 / 1 실패


#endif