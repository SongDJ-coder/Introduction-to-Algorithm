/* ch9_3/main.c — SELECT (median of medians) 테스트 하네스
 *
 * 계약:
 *   int three_way_select(int *arr, size_t n, int i, int *result);
 *     - i 는 1-based  (i = 1 이 최솟값)
 *     - 반환 0 성공 / 0 이 아닌 값 실패
 *     - 값은 *result 로 나온다
 *     - arr 을 재배치한다 (qsort 방식)
 *
 * 3중 검사
 *   ① 반환값이 기대(성공/실패)와 일치하는가
 *   ② 값이 기준 정답(qsort 한 사본의 [i-1])과 일치하는가
 *   ③ 다중집합이 보존되었는가
 *
 * 9.2 하네스와 같은 구조지만 **중복 케이스를 훨씬 두껍게** 넣었다.
 * 3-way partition 을 쓰는 이유가 중복이므로, 거기가 이 절의 급소다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

#include "three_way.h"

static int g_pass = 0;
static int g_fail = 0;

/* ------------------------------------------------------------------ */

/* 뺄셈으로 비교하면 INT_MIN/INT_MAX 에서 오버플로한다 */
static int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

static int *dup_array(const int *src, int n)
{
    int *dst;

    if (n <= 0)
        return NULL;

    dst = malloc((size_t)n * sizeof *dst);
    if (dst == NULL) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
    memcpy(dst, src, (size_t)n * sizeof *dst);
    return dst;
}

static int is_permutation_of(const int *a, const int *b, int n)
{
    int *sa, *sb, eq;

    if (n <= 0)
        return 1;

    sa = dup_array(a, n);
    sb = dup_array(b, n);
    qsort(sa, (size_t)n, sizeof *sa, cmp_int);
    qsort(sb, (size_t)n, sizeof *sb, cmp_int);
    eq = (memcmp(sa, sb, (size_t)n * sizeof *sa) == 0);

    free(sa);
    free(sb);
    return eq;
}

static int reference_select(const int *arr, int n, int i)
{
    int *copy = dup_array(arr, n);
    int val;

    qsort(copy, (size_t)n, sizeof *copy, cmp_int);
    val = copy[i - 1];
    free(copy);
    return val;
}

static void print_array(const int *a, int n)
{
    int j;
    printf("[");
    for (j = 0; j < n && j < 12; j++)
        printf("%s%d", j ? "," : "", a[j]);
    if (n > 12)
        printf(",...");
    printf("]");
}

/* ------------------------------------------------------------------ */

static void run_case(const char *name, const int *input, int n, int i, int expect_ok)
{
    int *work = dup_array(input, n);
    int got = -424242;
    int rc, ok = 1;
    const char *why = "";
    int want = 0;

    rc = three_way_select(work, (size_t)n, i, &got);

    if (expect_ok && rc != 0)       { ok = 0; why = "성공을 기대했는데 실패 반환"; }
    else if (!expect_ok && rc == 0) { ok = 0; why = "실패를 기대했는데 성공 반환"; }

    if (ok && expect_ok) {
        want = reference_select(input, n, i);
        if (got != want) { ok = 0; why = "값이 기준 정답과 다름"; }
    }

    if (ok && !is_permutation_of(work, input, n)) {
        ok = 0; why = "다중집합이 깨짐 (원소가 사라지거나 덮어써짐)";
    }

    if (ok) {
        g_pass++;
        printf("  PASS  %-30s n=%-5d i=%-5d", name, n, i);
        if (expect_ok) printf(" -> %d", got);
        else           printf(" -> rejected");
        printf("\n");
    } else {
        g_fail++;
        printf("  FAIL  %-30s n=%-5d i=%-5d  %s\n", name, n, i, why);
        printf("        input  "); print_array(input, n); printf("\n");
        printf("        after  "); print_array(work, n);  printf("\n");
        if (expect_ok) printf("        got %d, want %d, rc %d\n", got, want, rc);
        else           printf("        rc %d\n", rc);
    }

    free(work);
}

static void run_all_ranks(const char *name, const int *input, int n)
{
    int i;
    for (i = 1; i <= n; i++)
        run_case(name, input, n, i, 1);
}

/* ------------------------------------------------------------------ */

static void fixed_tests(void)
{
    static const int random8[]  = {2, 8, 7, 1, 3, 5, 6, 4};
    static const int sorted12[] = {1,2,3,4,5,6,7,8,9,10,11,12};
    static const int rev12[]    = {12,11,10,9,8,7,6,5,4,3,2,1};
    static const int alleq10[]  = {3,3,3,3,3,3,3,3,3,3};      /* Lomuto 를 터뜨리는 케이스 */
    static const int twoval12[] = {5,9,5,9,5,9,5,9,5,9,5,9};
    static const int one[]      = {7};
    static const int two[]      = {9, 4};
    static const int five[]     = {4, 9, 1, 1, 7};
    static const int six[]      = {4, 9, 1, 1, 7, 0};
    static const int extremes[] = {INT_MAX, 0, INT_MIN, -1, 1, INT_MAX, INT_MIN};
    static const int negs[]     = {-5,-1,-9,-3,-7,-2,-8};
    static const int mixdup[]   = {3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4};

    int dummy = 0;
    int probe[4] = {1, 2, 3, 4};

    puts("[1] 정확성 — 각 배열에 대해 i = 1..n 전부");

    run_all_ranks("무작위 8개",            random8,  8);
    run_all_ranks("이미 정렬 12개",        sorted12, 12);
    run_all_ranks("역순 12개",             rev12,    12);
    run_all_ranks("전부 중복 (3 x10)",     alleq10,  10);
    run_all_ranks("값 두 종류 (5,9)",      twoval12, 12);
    run_all_ranks("원소 1개",              one,      1);
    run_all_ranks("원소 2개",              two,      2);
    run_all_ranks("원소 5개 (기저 경계)",  five,     5);
    run_all_ranks("원소 6개 (첫 재귀)",    six,      6);
    run_all_ranks("INT_MIN/INT_MAX 중복",  extremes, 7);
    run_all_ranks("전부 음수",             negs,     7);
    run_all_ranks("중복 다수 20개",        mixdup,   20);

    puts("");
    puts("[2] 범위 밖 i — 크래시가 아니라 실패 반환이어야 한다");

    run_case("i = 0",       random8, 8, 0, 0);
    run_case("i = -1",      random8, 8, -1, 0);
    run_case("i = n+1",     random8, 8, 9, 0);
    run_case("i = INT_MAX", random8, 8, INT_MAX, 0);
    run_case("n = 0",       random8, 0, 1, 0);

    puts("");
    puts("[3] NULL 인자 · size_t -> int 좁히기");

    if (three_way_select(NULL, 4, 1, &dummy) != 0) {
        g_pass++; puts("  PASS  arr == NULL                  -> rejected");
    } else { g_fail++; puts("  FAIL  arr == NULL 을 통과시킴"); }

    if (three_way_select(probe, 4, 1, NULL) != 0) {
        g_pass++; puts("  PASS  result == NULL               -> rejected");
    } else { g_fail++; puts("  FAIL  result == NULL 을 통과시킴"); }

    if (three_way_select(probe, (size_t)INT_MAX + 1, 1, &dummy) != 0) {
        g_pass++; puts("  PASS  n = INT_MAX + 1              -> rejected");
    } else { g_fail++; puts("  FAIL  int 범위를 넘는 n 을 통과시킴"); }

    if (three_way_select(probe, SIZE_MAX, 1, &dummy) != 0) {
        g_pass++; puts("  PASS  n = SIZE_MAX                 -> rejected");
    } else { g_fail++; puts("  FAIL  n = SIZE_MAX 를 통과시킴"); }
}

/* ------------------------------------------------------------------ */

/* spread 를 좁게 잡으면 중복이 폭증한다.
 * 3-way partition 의 존재 이유가 중복이므로 여기를 두껍게 훑는다. */
static void stress_tests(int max_n, int rounds)
{
    int n, round, i, spread;
    int local_pass = 0, local_fail = 0;

    printf("[4] 무작위 스트레스 — n = 1..%d, 각 n 마다 %d회, 매회 i = 1..n 전부\n",
           max_n, rounds);
    printf("     값 범위를 1(전부 같음) / 2 / 3 / n / 4n+7 로 바꿔가며 중복 밀도를 흔든다\n");

    for (n = 1; n <= max_n; n++) {
        for (round = 0; round < rounds; round++) {

            int *input, *work;

            switch (round % 5) {
            case 0:  spread = 1;         break;   /* 전부 같은 값 */
            case 1:  spread = 2;         break;
            case 2:  spread = 3;         break;
            case 3:  spread = n;         break;
            default: spread = 4 * n + 7; break;
            }

            input = malloc((size_t)n * sizeof *input);
            if (input == NULL) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
            for (i = 0; i < n; i++)
                input[i] = rand() % spread - spread / 2;

            for (i = 1; i <= n; i++) {
                int got = 0, want, rc, ok = 1;

                work = dup_array(input, n);
                rc = three_way_select(work, (size_t)n, i, &got);
                want = reference_select(input, n, i);

                if (rc != 0)                                 ok = 0;
                else if (got != want)                        ok = 0;
                else if (!is_permutation_of(work, input, n)) ok = 0;

                if (ok) {
                    local_pass++;
                } else {
                    local_fail++;
                    if (local_fail <= 5) {
                        printf("  FAIL  n=%d i=%d spread=%d rc=%d got=%d want=%d\n",
                               n, i, spread, rc, got, want);
                        printf("        input "); print_array(input, n); printf("\n");
                        printf("        after "); print_array(work, n);  printf("\n");
                    }
                }
                free(work);
            }
            free(input);
        }
    }

    g_pass += local_pass;
    g_fail += local_fail;
    printf("  %d / %d PASS\n", local_pass, local_pass + local_fail);
}

/* ------------------------------------------------------------------ */

int main(void)
{
    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1234567890u; */   /* FAIL 재현 시 이 줄로 바꾼다 */

    srand(seed);   /* 프로그램 전체에서 딱 한 번 */

    printf("=== SELECT (median of medians) test ===\n");
    printf("seed = %u   (재현하려면 이 값을 srand 에 박는다)\n\n", seed);

    fixed_tests();
    puts("");
    stress_tests(60, 10);

    printf("\n=== %d / %d PASS", g_pass, g_pass + g_fail);
    if (g_fail)
        printf(",  %d FAIL", g_fail);
    printf(" ===\n");

    return g_fail ? 1 : 0;
}
