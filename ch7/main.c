#include "quicksort.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static void fill_sorted(int *arr, int n)
{
    for (int i = 0; i < n; i++) arr[i] = i;
}

static void fill_reverse(int *arr, int n)
{
    for (int i = 0; i < n; i++) arr[i] = n - i;
}

int is_sorted(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
        if (arr[i] > arr[i+1]) return 0;
    return 1;
}

// 알고리즘 하나를 측정
static void measure(const char *name,
                    void (*sort)(int*, int, int),
                    void (*fill)(int*, int),
                    int *arr, int n)
{
    fill(arr, n);
    reset_depth();

    clock_t start = clock();
    sort(arr, 0, n - 1);
    double sec = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("%-18s depth=%-8d %.3f sec   sorted=%d\n",
           name, get_max_depth(), sec, is_sorted(arr, n));
}

int main(void)
{
    int n = 200000;
    int *arr = malloc(sizeof(int) * n);
    if (arr == NULL) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    printf("=== 정렬된 배열 (n=%d) ===\n", n);
    measure("quicksort_tail", quicksort_tail, fill_sorted, arr, n);
    measure("quicksort_opt",  quicksort_opt,  fill_sorted, arr, n);

    printf("\n=== 역순 배열 (n=%d) ===\n", n);
    measure("quicksort_tail", quicksort_tail, fill_reverse, arr, n);
    measure("quicksort_opt",  quicksort_opt,  fill_reverse, arr, n);

    free(arr);
    return 0;
}