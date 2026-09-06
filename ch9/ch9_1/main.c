/* ch9_1 / main.c  —  min_max 검증 하네스
 *
 * 이 파일은 검증 도구다. 알고리즘은 min_max.c 에만 있다.
 *
 * ------------------------------------------------------------------
 * 이 하네스가 링크되려면 min_max 쪽에 전역 카운터가 있어야 한다.
 *
 *   min_max.h :  #include <stddef.h>
 *                extern size_t g_comparisons;
 *
 *   min_max.c :  size_t g_comparisons = 0;
 *                → 배열 원소끼리 비교하는 곳마다 g_comparisons++
 *                  (짝 비교 1회 + s vs min 1회 + l vs max 1회 = 짝당 3회)
 *                → 초기화에서 실제로 비교를 했을 때만 센다
 *                → 함수 안에서 0으로 리셋하지 말 것. 리셋은 호출자 몫이다
 *
 * 그리고 min_max.c 안의 printf 는 지운다. 라이브러리는 출력하지 않는다.
 * ------------------------------------------------------------------
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "min_max.h"

#define LEN(a)   (sizeof(a) / sizeof((a)[0]))
#define SENTINEL 0x5A5A5A5A

typedef struct {
    const char *name;
    const int  *arr;
    size_t      n;
    int         want_ret;   /* 0 성공 / 1 실패 */
    int         want_min;   /* want_ret == 0 일 때만 의미 있다 */
    int         want_max;
} Case;

/* 교재가 말하는 상한: 3*floor(n/2) */
static size_t comparison_bound(size_t n)
{
    return 3 * (n / 2);
}

/* 이 알고리즘의 비교 횟수는 입력과 무관하게 n 하나로 결정된다.
 * if 의 결과에 따라 달라지는 것은 '대입 여부'지 '비교 여부'가 아니다.
 * 따라서 상한 이하인지가 아니라 '정확히 이 값인지'를 검사해야 한다.
 *
 *   n = 0, 1 : 0                     비교 없이 끝난다
 *   n 홀수   : 3*(n-1)/2             초기화 0회 + 짝 (n-1)/2개 * 3회
 *   n 짝수   : 3*n/2 - 2             초기화 1회 + 짝 (n-2)/2개 * 3회
 *
 * 상한만 검사하면 과소 계수(0회 같은 것)가 그대로 통과한다. */
static size_t expected_comparisons(size_t n)
{
    if (n < 2)     return 0;
    if (n % 2 == 0) return 3 * n / 2 - 2;
    return 3 * (n - 1) / 2;
}

/* 입력 훼손 검사용 */
static int arrays_equal(const int *a, const int *b, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (a[i] != b[i])
            return 0;
    return 1;
}

/* 독립적인 기준 정답. naive 2n-2 스캔 — min_max 와 로직을 공유하지 않는다 */
static void reference_min_max(const int *a, size_t n, int *mn, int *mx)
{
    *mn = a[0];
    *mx = a[0];
    for (size_t i = 1; i < n; i++) {
        if (a[i] < *mn) *mn = a[i];
        if (a[i] > *mx) *mx = a[i];
    }
}

/* ---------------------------------------------------------------- */

static int run_case(const Case *c)
{
    int    min = SENTINEL, max = SENTINEL;
    int   *snapshot = NULL;
    int    ok = 1;
    size_t cap  = comparison_bound(c->n);
    size_t want = expected_comparisons(c->n);

    if (c->n > 0) {
        snapshot = malloc(c->n * sizeof(int));
        if (snapshot == NULL) {
            printf("  [ERROR] snapshot malloc 실패\n");
            return 0;
        }
        for (size_t i = 0; i < c->n; i++)
            snapshot[i] = c->arr[i];
    }

    g_comparisons = 0;
    int    ret  = min_max(c->arr, c->n, &min, &max);
    size_t used = g_comparisons;

    printf("%-24s n=%-3zu ", c->name, c->n);

    /* 1. 반환값 */
    if (ret != c->want_ret) {
        printf("\n    [FAIL] 반환값 %d, 기대 %d", ret, c->want_ret);
        ok = 0;
    }

    if (c->want_ret == 0) {
        /* 2. 값 */
        if (min != c->want_min) {
            printf("\n    [FAIL] min = %d, 기대 %d", min, c->want_min);
            ok = 0;
        }
        if (max != c->want_max) {
            printf("\n    [FAIL] max = %d, 기대 %d", max, c->want_max);
            ok = 0;
        }
        /* 3. 비교 횟수 — 정확히 일치해야 한다 */
        if (used != want) {
            printf("\n    [FAIL] 비교 %zu회, 기대 %zu회 (%s). 교재 상한은 %zu",
                   used, want,
                   used < want ? "과소 계수 — 세지 않은 비교가 있다"
                               : "초과 — 불필요한 비교가 있다",
                   cap);
            ok = 0;
        }
    } else {
        /* 4. 실패 반환이면 출력 슬롯을 건드리지 않아야 한다 */
        if (min != SENTINEL || max != SENTINEL) {
            printf("\n    [FAIL] 실패 반환인데 출력 슬롯이 덮어써짐 "
                   "(min=%d, max=%d)", min, max);
            ok = 0;
        }
    }

    /* 5. 입력 불변 */
    if (snapshot != NULL && !arrays_equal(c->arr, snapshot, c->n)) {
        printf("\n    [FAIL] 입력 배열이 훼손됨");
        ok = 0;
    }

    if (ok)
        printf("PASS   비교 %zu (기대 %zu, 교재 상한 %zu)\n", used, want, cap);
    else
        printf("\n");

    free(snapshot);
    return ok;
}

/* 무작위 대량 검증. 기준 정답은 reference_min_max */
static int stress(size_t max_n, int trials_per_n)
{
    static int buf[256];
    int fails = 0;

    for (size_t n = 1; n <= max_n; n++) {
        for (int t = 0; t < trials_per_n; t++) {
            for (size_t i = 0; i < n; i++)
                buf[i] = (rand() % 2001) - 1000;

            int want_min, want_max;
            reference_min_max(buf, n, &want_min, &want_max);

            int min = SENTINEL, max = SENTINEL;
            g_comparisons = 0;
            int    ret  = min_max(buf, n, &min, &max);
            size_t used = g_comparisons;
            size_t want = expected_comparisons(n);

            if (ret != 0 || min != want_min || max != want_max || used != want) {
                printf("  [FAIL] n=%zu t=%d : ret=%d  min=%d(기대 %d)  "
                       "max=%d(기대 %d)  비교=%zu(기대 %zu)\n",
                       n, t, ret, min, want_min, max, want_max, used, want);
                if (++fails >= 10) {
                    printf("  ... 실패가 너무 많아 중단\n");
                    return fails;
                }
            }
        }
    }
    return fails;
}

/* ---------------------------------------------------------------- */

int main(void)
{
    static const int a_random[]  = {2, 8, 7, 1, 3, 5, 6, 4};
    static const int a_sorted[]  = {1, 2, 3, 4, 5};
    static const int a_reverse[] = {5, 4, 3, 2, 1};
    static const int a_same[]    = {3, 3, 3, 3, 3};
    static const int a_single[]  = {7};
    static const int a_two[]     = {9, 4};
    static const int a_two_asc[] = {4, 9};
    static const int a_three[]   = {4, 9, 1};
    static const int a_seven[]   = {6, 2, 9, 4, 1, 8, 3};
    static const int a_extreme[] = {0, INT_MAX, INT_MIN, 5};
    static const int a_head[]    = {1, 9, 5, 5, 5};      /* min 맨 앞 */
    static const int a_tail[]    = {5, 5, 5, 1, 9};      /* max 맨 뒤 */
    static const int a_pair[]    = {5, 5, 1, 9, 5, 5};   /* min·max 가 같은 짝 */
    static const int a_negative[]= {-4, -9, -1, -7};

    const Case cases[] = {
        {"무작위",             a_random,   LEN(a_random),   0,  1,       8},
        {"이미 정렬",          a_sorted,   LEN(a_sorted),   0,  1,       5},
        {"역순",               a_reverse,  LEN(a_reverse),  0,  1,       5},
        {"전부 중복",          a_same,     LEN(a_same),     0,  3,       3},
        {"원소 1개",           a_single,   LEN(a_single),   0,  7,       7},
        {"n=2 내림",           a_two,      LEN(a_two),      0,  4,       9},
        {"n=2 오름",           a_two_asc,  LEN(a_two_asc),  0,  4,       9},
        {"n=3 홀수",           a_three,    LEN(a_three),    0,  1,       9},
        {"n=7 홀수",           a_seven,    LEN(a_seven),    0,  1,       9},
        {"INT_MIN / INT_MAX",  a_extreme,  LEN(a_extreme),  0,  INT_MIN, INT_MAX},
        {"min 이 맨 앞",       a_head,     LEN(a_head),     0,  1,       9},
        {"max 가 맨 뒤",       a_tail,     LEN(a_tail),     0,  1,       9},
        {"min·max 가 같은 짝", a_pair,     LEN(a_pair),     0,  1,       9},
        {"전부 음수",          a_negative, LEN(a_negative), 0, -9,      -1},
        {"빈 배열",            NULL,       0,               1,  0,       0},
    };

    size_t total  = LEN(cases);
    size_t passed = 0;

    printf("=== 고정 케이스 ===\n");
    for (size_t i = 0; i < total; i++)
        passed += (size_t)run_case(&cases[i]);

    printf("\n고정 케이스: %zu/%zu PASS\n", passed, total);

    printf("\n=== 무작위 대량 검증 (n = 1..128, 각 200회) ===\n");
    srand(20260906u);
    int stress_fails = stress(128, 200);
    if (stress_fails == 0)
        printf("25600/25600 PASS\n");
    else
        printf("실패 %d건\n", stress_fails);

    int all_ok = (passed == total) && (stress_fails == 0);
    printf("\n%s\n", all_ok ? "=== 전체 PASS ===" : "=== 실패 있음 ===");
    return all_ok ? 0 : 1;
}
