#include "radix_sort.h"
#include "counting_sort.h"
#include <stdio.h>
#include <stdlib.h>

/* ── 검증 헬퍼 ─────────────────────────────────────────────
   알고리즘이 아니라 결과를 확인하는 도구.                       */

#define SENTINEL (-999)   /* 입력에 절대 안 나올 값. 함수가 안 건드린 칸을 드러낸다 */

static void print_arr(const char *label, const int *arr, size_t n)
{
    printf("%-14s", label);
    for (size_t i = 0; i < n; i++) printf(" %d", arr[i]);
    printf("\n");
}

static int is_sorted(const int *arr, size_t n)
{
    for (size_t i = 0; i + 1 < n; i++)
        if (arr[i] > arr[i+1]) return 0;
    return 1;
}

static int arrays_equal(const int *a, const int *b, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

/* 정렬 결과가 원본의 순열인지 확인.
   radix sort는 원소를 옮기기만 해야 한다. 값이 사라지거나 복제되면
   핑퐁에서 한쪽 버퍼를 덮어썼다는 신호다. */
static int is_permutation_of(const int *out, const int *in, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        size_t c_in = 0, c_out = 0;
        for (size_t j = 0; j < n; j++)
        {
            if (in[j]  == in[i]) c_in++;
            if (out[j] == in[i]) c_out++;
        }
        if (c_in != c_out) return 0;
    }
    return 1;
}

/* 안 건드린 칸이 남아 있는지. SENTINEL이 하나라도 보이면 그 칸은 미기록. */
static int all_written(const int *arr, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (arr[i] == SENTINEL) return 0;
    return 1;
}

/* ── 하네스 ────────────────────────────────────────────────
   케이스 하나의 전 과정: 할당 → 오염 → 호출 → 4중 검사 → 해제. */

static int g_pass = 0, g_fail = 0;

static void run_test(const char *name, const int *input, size_t n,
                     const int *expected)
{
    int *result = malloc(n > 0 ? n * sizeof(int) : 1);
    if (result == NULL)
    {
        printf("[%-14s] ALLOC FAIL\n", name);
        g_fail++;
        return;
    }
    for (size_t i = 0; i < n; i++) result[i] = SENTINEL;

    int rc = radix_sort(input, n, result);
    if (rc != 0)
    {
        printf("[%-14s] RETURN %d\n", name, rc);
        g_fail++;
        free(result);
        return;
    }

    int w = all_written(result, n);
    int e = arrays_equal(result, expected, n);
    int s = is_sorted(result, n);
    int p = is_permutation_of(result, input, n);

    if (w && e && s && p)
    {
        printf("[%-14s] PASS  ", name);
        for (size_t i = 0; i < n; i++) printf(" %d", result[i]);
        printf("\n");
        g_pass++;
    }
    else
    {
        printf("[%-14s] FAIL  written=%d equal=%d sorted=%d perm=%d\n",
               name, w, e, s, p);
        print_arr("  got", result, n);
        print_arr("  want", expected, n);
        g_fail++;
    }

    free(result);
}

int main(void)
{
    /* 핑퐁 경로: num = 0,1,2,3,4,5 전부 */

    int a1[] = {3, 2, 5, 7, 4, 3, 1, 8, 0, 6, 3, 10, 19};
    int e1[] = {0, 1, 2, 3, 3, 3, 4, 5, 6, 7, 8, 10, 19};
    run_test("1 num=2", a1, 13, e1);

    int a2[] = {3, 0, 0, 7, 4, 0, 1, 8, 0, 6, 3, 1, 9};
    int e2[] = {0, 0, 0, 0, 1, 1, 3, 3, 4, 6, 7, 8, 9};
    run_test("2 num=1", a2, 13, e2);

    int a3[] = {100, 5, 42, 999, 7, 250};
    int e3[] = {5, 7, 42, 100, 250, 999};
    run_test("3 num=3", a3, 6, e3);

    int a4[] = {1000, 9999, 123, 4, 56, 7890};
    int e4[] = {4, 56, 123, 1000, 7890, 9999};
    run_test("4 num=4", a4, 6, e4);

    int a5[] = {3, 0, 0, 7, 4, 30000, 1, 8, 0, 6, 3, 10, 19};
    int e5[] = {0, 0, 0, 1, 3, 3, 4, 6, 7, 8, 10, 19, 30000};
    run_test("5 num=5", a5, 13, e5);

    /* num = 0 경로. if(num == 0) num++ 를 찌른다.
       정답이 전부 0이라 SENTINEL 없이는 "안 썼다"와 구별이 안 된다. */
    int a6[] = {0, 0, 0, 0};
    int e6[] = {0, 0, 0, 0};
    run_test("6 num=0", a6, 4, e6);

    /* 경계 */

    int a7[] = {7};
    int e7[] = {7};
    run_test("7 n=1", a7, 1, e7);

    int a8[] = {0};              /* n=0 이라 내용은 안 읽힌다 */
    int e8[] = {0};
    run_test("8 n=0", a8, 0, e8);

    int a9[] = {7, 7, 7, 7};
    int e9[] = {7, 7, 7, 7};
    run_test("9 all same", a9, 4, e9);

    /* 축퇴된 입력 */

    int a10[] = {1, 2, 3, 4, 5};
    int e10[] = {1, 2, 3, 4, 5};
    run_test("10 sorted", a10, 5, e10);

    int a11[] = {50, 40, 30, 20, 10};
    int e11[] = {10, 20, 30, 40, 50};
    run_test("11 reverse", a11, 5, e11);

    /* 자릿수 경계값. 10, 100, 1000 이 자릿수 증가 지점 */
    int a12[] = {9, 10, 99, 100, 1000};
    int e12[] = {9, 10, 99, 100, 1000};
    run_test("12 pow10", a12, 5, e12);

    /* 자릿수 폭이 다른 중복값 */
    int a13[] = {11, 1, 111, 11, 1};
    int e13[] = {1, 1, 11, 11, 111};
    run_test("13 dup mixed", a13, 5, e13);

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
