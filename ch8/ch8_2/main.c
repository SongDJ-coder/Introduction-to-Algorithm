#include "counting_sort.h"
#include <stdio.h>
#include <stdlib.h>

static void run_test(const char *name, const int *input, size_t n, int k,
                     const int *expected)
{
    int *result = malloc(n > 0 ? n * sizeof(int) : 1);
    if (result == NULL) {
        printf("[%s] ALLOC FAIL\n", name);
        return;
    }

    int rc = counting_sort(input, n, result, k);
    if (rc != 0) {
        printf("[%s] RETURN %d\n", name, rc);
        free(result);
        return;
    }

    int ok = 1;
    for (size_t i = 0; i < n; i++)
        if (result[i] != expected[i]) ok = 0;

    printf("[%s] %s   got:", name, ok ? "PASS" : "FAIL");
    for (size_t i = 0; i < n; i++) printf(" %d", result[i]);
    if (!ok) {
        printf("   want:");
        for (size_t i = 0; i < n; i++) printf(" %d", expected[i]);
    }
    printf("\n");

    free(result);
}

int main(void)
{
    int a1[] = {2, 5, 3, 0, 2, 3, 0, 3};
    int e1[] = {0, 0, 2, 2, 3, 3, 3, 5};
    run_test("1 mixed", a1, 8, 5, e1);

    int a2[] = {5, 4, 3, 2, 1, 0};
    int e2[] = {0, 1, 2, 3, 4, 5};
    run_test("2 reverse", a2, 6, 5, e2);

    int a3[] = {0, 1, 2, 3, 4, 5};
    int e3[] = {0, 1, 2, 3, 4, 5};
    run_test("3 sorted", a3, 6, 5, e3);

    int a4[] = {0, 0, 0, 0};
    int e4[] = {0, 0, 0, 0};
    run_test("4 k=0", a4, 4, 0, e4);

    int a5[] = {7};
    int e5[] = {7};
    run_test("5 single", a5, 1, 7, e5);

    int a6[] = {3, 3, 3, 3, 3};
    int e6[] = {3, 3, 3, 3, 3};
    run_test("6 all same", a6, 5, 3, e6);

    int a7[] = {0, 5};
    int e7[] = {0, 5};
    run_test("7 endpoints", a7, 2, 5, e7);

    int a8[] = {2, 2, 2, 2};
    int e8[] = {2, 2, 2, 2};
    run_test("8 below k", a8, 4, 5, e8);

    int a9[] = {0, 0, 5, 5};
    int e9[] = {0, 0, 5, 5};
    run_test("9 gap", a9, 4, 5, e9);

    int a10[] = {3, 1, 4, 1, 5, 9, 2, 6};
    int e10[] = {1, 1, 2, 3, 4, 5, 6, 9};
    run_test("10 k>n", a10, 8, 9, e10);

    return 0;
}
