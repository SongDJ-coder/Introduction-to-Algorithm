/* ch9_2/main.c — RANDOMIZED-SELECT 테스트 하네스
 *
 * 전제하는 계약 (9.2 세션에서 확정):
 *   int randomized_select(int *arr, size_t n, int i, int *result);
 *     - i 는 1-based  (i = 1 이 최솟값)
 *     - 반환 0 성공 / 0 이 아닌 값 실패
 *     - 값은 *result 로 나온다
 *     - arr 을 재배치한다 (qsort 방식). 원본 보존은 호출자 책임
 *     - p, r 은 공개 API 에 없다. 진입점 내부에서만 존재한다
 *
 * 3중 검사
 *   ① 반환값이 기대(성공/실패)와 일치하는가
 *   ② 값이 기준 정답(qsort 한 사본의 [i-1])과 일치하는가
 *   ③ 다중집합이 보존되었는가  ← snapshot(입력 불변) 대신 쓰는 검사
 *
 * ③이 이 절의 핵심이다. 배열을 재배치하는 걸 허용했으므로
 * "입력이 그대로인가"는 물을 수 없다. 대신 "원소가 사라지거나
 * 늘어나지 않았는가"를 묻는다. partition 이 값을 덮어쓰는 버그는 여기서 잡힌다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

#include "randomized_select.h"

static int g_pass = 0;
static int g_fail = 0;

/* ------------------------------------------------------------------ */
/* 유틸                                                                */
/* ------------------------------------------------------------------ */

/* 뺄셈으로 비교하면 INT_MIN/INT_MAX 케이스에서 오버플로한다.
 * 이 하네스는 그 두 값을 실제로 테스트하므로 뺄셈을 쓸 수 없다. */
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
    if (dst == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(dst, src, (size_t)n * sizeof *dst);
    return dst;
}

/* 두 배열이 같은 다중집합인가 (순서 무관, 중복 개수까지 일치) */
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

/* 기준 정답: 사본을 정렬해서 i-1 번째 (i 는 1-based) */
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
/* 케이스 실행                                                          */
/* ------------------------------------------------------------------ */

/* expect_ok = 1 이면 성공(0 반환)을 기대, 0 이면 실패(0 아닌 값)를 기대 */
static void run_case(const char *name, const int *input, int n, int i, int expect_ok)
{
    int *work = dup_array(input, n);
    int got = -424242;         /* 센티널: 함수가 안 건드리면 그대로 남는다 */
    int rc, ok = 1;
    const char *why = "";
    int want = 0;

    rc = randomized_select(work, (size_t)n, i, &got);

    /* ① 반환값 */
    if (expect_ok && rc != 0) {
        ok = 0; why = "성공을 기대했는데 실패 반환";
    } else if (!expect_ok && rc == 0) {
        ok = 0; why = "실패를 기대했는데 성공 반환";
    }

    /* ② 값 */
    if (ok && expect_ok) {
        want = reference_select(input, n, i);
        if (got != want) {
            ok = 0; why = "값이 기준 정답과 다름";
        }
    }

    /* ③ 다중집합 보존 */
    if (ok && !is_permutation_of(work, input, n)) {
        ok = 0; why = "다중집합이 깨짐 (원소가 사라지거나 덮어써짐)";
    }

    if (ok) {
        g_pass++;
        printf("  PASS  %-34s n=%-4d i=%-4d", name, n, i);
        if (expect_ok) printf(" -> %d", got);
        else           printf(" -> rejected");
        printf("\n");
    } else {
        g_fail++;
        printf("  FAIL  %-34s n=%-4d i=%-4d  %s\n", name, n, i, why);
        printf("        input  "); print_array(input, n); printf("\n");
        printf("        after  "); print_array(work, n);  printf("\n");
        if (expect_ok)
            printf("        got %d, want %d, rc %d\n", got, want, rc);
        else
            printf("        rc %d\n", rc);
    }

    free(work);
}

/* 배열 하나에 대해 i = 1..n 을 전부 훑는다 */
static void run_all_ranks(const char *name, const int *input, int n)
{
    int i;
    for (i = 1; i <= n; i++)
        run_case(name, input, n, i, 1);
}

/* ------------------------------------------------------------------ */
/* 고정 케이스                                                          */
/* ------------------------------------------------------------------ */

static void fixed_tests(void)
{
    static const int random8[]  = {2, 8, 7, 1, 3, 5, 6, 4};
    static const int sorted5[]  = {1, 2, 3, 4, 5};
    static const int rev5[]     = {5, 4, 3, 2, 1};
    static const int dup5[]     = {3, 3, 3, 3, 3};
    static const int one[]      = {7};
    static const int two[]      = {9, 4};
    static const int three[]    = {4, 9, 1};
    static const int extremes[] = {INT_MAX, 0, INT_MIN, -1, 1};
    static const int negs[]     = {-5, -1, -9, -3, -7};
    static const int mixdup[]   = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

    int dummy = 0;
    int probe[4] = {1, 2, 3, 4};

    puts("[1] 정확성 — 각 배열에 대해 i = 1..n 전부");

    run_all_ranks("무작위 8개", random8, 8);
    run_all_ranks("이미 정렬", sorted5, 5);
    run_all_ranks("역순", rev5, 5);
    run_all_ranks("전부 중복", dup5, 5);
    run_all_ranks("원소 1개", one, 1);
    run_all_ranks("원소 2개", two, 2);
    run_all_ranks("원소 3개", three, 3);
    run_all_ranks("INT_MIN/INT_MAX 포함", extremes, 5);
    run_all_ranks("전부 음수", negs, 5);
    run_all_ranks("중복 다수", mixdup, 11);

    puts("");
    puts("[2] 범위 밖 i — 크래시가 아니라 실패 반환이어야 한다");

    run_case("i = 0",        random8, 8,  0, 0);
    run_case("i = -1",       random8, 8, -1, 0);
    run_case("i = n+1",      random8, 8,  9, 0);
    run_case("i = 1000",     random8, 8, 1000, 0);
    run_case("i = INT_MAX",  random8, 8, INT_MAX, 0);
    run_case("n = 0",        random8, 0,  1, 0);   /* 빈 배열 */

    puts("");
    puts("[3] NULL 인자");

    if (randomized_select(NULL, 4, 1, &dummy) != 0) {
        g_pass++; puts("  PASS  arr == NULL                     -> rejected");
    } else {
        g_fail++; puts("  FAIL  arr == NULL 을 통과시킴");
    }

    if (randomized_select(probe, 4, 1, NULL) != 0) {
        g_pass++; puts("  PASS  result == NULL                  -> rejected");
    } else {
        g_fail++; puts("  FAIL  result == NULL 을 통과시킴");
    }

    puts("");
    puts("[4] size_t -> int 좁히기");
    /* 헬퍼가 int p, int r 을 쓰므로 n 이 INT_MAX 를 넘으면 (int)n 에서 값이 깨진다.
     * 배열을 만지기 전에 거부해야 한다. 검증이 없으면 여기서 죽거나 ASan 이 잡는다. */

    if (randomized_select(probe, (size_t)INT_MAX + 1, 1, &dummy) != 0) {
        g_pass++; puts("  PASS  n = INT_MAX + 1                 -> rejected");
    } else {
        g_fail++; puts("  FAIL  int 범위를 넘는 n 을 통과시킴");
    }

    if (randomized_select(probe, SIZE_MAX, 1, &dummy) != 0) {
        g_pass++; puts("  PASS  n = SIZE_MAX                    -> rejected");
    } else {
        g_fail++; puts("  FAIL  n = SIZE_MAX 를 통과시킴");
    }
}

/* ------------------------------------------------------------------ */
/* 무작위 스트레스                                                      */
/* ------------------------------------------------------------------ */

/* 값 범위를 좁게 잡아 중복이 자주 생기게 한다.
 * 중복이 없으면 순위 계산 버그가 안 드러나는 경우가 있다. */
static void stress_tests(int max_n, int rounds)
{
    int n, round, i, spread;
    int *input, *work;
    int local_pass = 0, local_fail = 0;

    printf("[5] 무작위 스트레스 — n = 1..%d, 각 n 마다 %d회, 매회 i = 1..n 전부\n",
           max_n, rounds);

    for (n = 1; n <= max_n; n++) {
        for (round = 0; round < rounds; round++) {

            /* 회차마다 값 범위를 바꾼다: 좁으면 중복 폭증, 넓으면 거의 유일 */
            spread = (round % 3 == 0) ? 3 : (round % 3 == 1) ? n : 4 * n + 7;

            input = malloc((size_t)n * sizeof *input);
            if (input == NULL) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
            for (i = 0; i < n; i++)
                input[i] = rand() % spread - spread / 2;

            for (i = 1; i <= n; i++) {
                int got = 0, want, rc, ok = 1;

                work = dup_array(input, n);
                rc = randomized_select(work, (size_t)n, i, &got);
                want = reference_select(input, n, i);

                if (rc != 0)                              ok = 0;
                else if (got != want)                     ok = 0;
                else if (!is_permutation_of(work, input, n)) ok = 0;

                if (ok) {
                    local_pass++;
                } else {
                    local_fail++;
                    if (local_fail <= 5) {   /* 처음 몇 건만 자세히 */
                        printf("  FAIL  n=%d i=%d rc=%d got=%d want=%d\n",
                               n, i, rc, got, want);
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
    /* 시드는 매 실행마다 다르되 반드시 출력한다.
     * FAIL 이 났을 때 이 값을 아래 srand 에 박아 넣으면 같은 실행을 재현할 수 있다.
     * 재현이 안 되는 무작위 테스트는 디버깅 도구로 쓸 수 없다. */
    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1234567890u; */   /* ← 재현할 때 이 줄로 바꾼다 */

    srand(seed);   /* 프로그램 전체에서 딱 한 번. 라이브러리 안에서 부르지 않는다 */

    printf("=== RANDOMIZED-SELECT test ===\n");
    printf("seed = %u   (재현하려면 이 값을 srand 에 박는다)\n\n", seed);

    fixed_tests();
    puts("");
    stress_tests(48, 12);

    printf("\n=== %d / %d PASS", g_pass, g_pass + g_fail);
    if (g_fail)
        printf(",  %d FAIL", g_fail);
    printf(" ===\n");

    return g_fail ? 1 : 0;
}
