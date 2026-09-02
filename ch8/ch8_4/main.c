/* main.c — bucket_sort 검증 하네스
 *
 * COWORK §7에 따라 Claude가 작성한 "검증 도구". 알고리즘 구현은 들어 있지 않다.
 * 기준 정답(oracle)은 표준 라이브러리 qsort로 만든다.
 *
 * 4중 검사 (8.3 main_radix.c 패턴)
 *   1) all_written        센티널이 남아 있으면 그 칸은 한 번도 안 쓰인 것
 *   2) arrays_equal       qsort 기준 정답과 완전 일치
 *   3) is_sorted          비내림차순
 *   4) is_permutation_of  원소 구성이 입력과 같은가 (중복 개수까지)
 *   + input_intact        입력 배열을 훼손하지 않았는가
 *
 * 빌드:
 *   cd ~/Projects/Algorithm/CLRS/ch8/ch8_4
 *   gcc -g -O0 -Wall -fsanitize=address bucketsort.c main.c \
 *       -o ../../bin/bucket_sort && ../../bin/bucket_sort
 */

#include "bucketsort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 입력에 절대 나타나지 않는 값. 안 쓰인 칸을 식별한다. */
#define SENTINEL (-987654.321)

static int tests_run = 0;
static int tests_passed = 0;

/* ---------- 기준 정답 ---------- */

static int cmp_double(const void *a, const void *b)
{
    double x = *(const double *)a;
    double y = *(const double *)b;
    if (x < y) return -1;
    if (x > y) return  1;
    return 0;
}

/* ---------- 검증 헬퍼 ---------- */

static int all_written(const double *a, int n)
{
    for (int i = 0; i < n; i++)
        if (a[i] == SENTINEL) return 0;
    return 1;
}

static int is_sorted(const double *a, int n)
{
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i]) return 0;
    return 1;
}

static int arrays_equal(const double *a, const double *b, int n)
{
    for (int i = 0; i < n; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

/* 양쪽을 각각 정렬해서 비교 — 중복 개수까지 같아야 통과 */
static int is_permutation_of(const double *a, const double *b, int n)
{
    if (n == 0) return 1;

    double *x = malloc((size_t)n * sizeof *x);
    double *y = malloc((size_t)n * sizeof *y);
    if (!x || !y) { fprintf(stderr, "OOM in is_permutation_of\n"); exit(1); }

    memcpy(x, a, (size_t)n * sizeof *x);
    memcpy(y, b, (size_t)n * sizeof *y);
    qsort(x, (size_t)n, sizeof *x, cmp_double);
    qsort(y, (size_t)n, sizeof *y, cmp_double);

    int ok = arrays_equal(x, y, n);
    free(x);
    free(y);
    return ok;
}

/* ---------- 출력 ---------- */

static void print_array(const char *label, const double *a, int n)
{
    int limit = n < 12 ? n : 12;
    printf("       %-8s [", label);
    for (int i = 0; i < limit; i++)
        printf("%s%g", i ? ", " : "", a[i]);
    if (limit < n) printf(", ... (%d개)", n);
    printf("]\n");
}

/* ---------- 테스트 실행기 ---------- */

static void run_case(const char *name, const double *input, int n)
{
    tests_run++;

    double *work   = NULL;  /* 입력 사본. 훼손 여부 확인용 */
    double *out    = NULL;
    double *expect = NULL;

    if (n > 0) {
        work   = malloc((size_t)n * sizeof *work);
        out    = malloc((size_t)n * sizeof *out);
        expect = malloc((size_t)n * sizeof *expect);
        if (!work || !out || !expect) { fprintf(stderr, "OOM in run_case\n"); exit(1); }

        memcpy(work,   input, (size_t)n * sizeof *work);
        memcpy(expect, input, (size_t)n * sizeof *expect);
        qsort(expect, (size_t)n, sizeof *expect, cmp_double);

        for (int i = 0; i < n; i++) out[i] = SENTINEL;
    }

    int rc = bucket_sort(work, out, n);

    int ok_rc      = (rc == 0);
    int ok_written = (n == 0) || all_written(out, n);
    int ok_sorted  = (n == 0) || is_sorted(out, n);
    int ok_perm    = (n == 0) || is_permutation_of(out, input, n);
    int ok_equal   = (n == 0) || arrays_equal(out, expect, n);
    int ok_intact  = (n == 0) || arrays_equal(work, input, n);

    int ok = ok_rc && ok_written && ok_sorted && ok_perm && ok_equal && ok_intact;
    if (ok) tests_passed++;

    printf("[%2d] %-28s n=%-5d %s\n", tests_run, name, n, ok ? "PASS" : "**FAIL**");

    if (!ok) {
        if (!ok_rc)      printf("       - 반환값이 0이 아님 (rc=%d)\n", rc);
        if (!ok_written) printf("       - 안 쓰인 칸이 있음 (센티널 잔존)\n");
        if (!ok_sorted)  printf("       - 정렬되지 않음\n");
        if (!ok_perm)    printf("       - 원소 구성이 입력과 다름 (값 유실/중복)\n");
        if (!ok_equal)   printf("       - qsort 기준 정답과 불일치\n");
        if (!ok_intact)  printf("       - 입력 배열이 훼손됨\n");

        print_array("input",  input,  n);
        print_array("output", out,    n);
        print_array("expect", expect, n);
    }

    free(work);
    free(out);
    free(expect);
}

/* ---------- 무작위 입력 생성 ---------- */

/* [0,1) 균등분포 — bucket sort의 전제가 성립하는 경우 */
static void case_uniform(int n)
{
    double *a = malloc((size_t)n * sizeof *a);
    if (!a) { fprintf(stderr, "OOM\n"); exit(1); }
    for (int i = 0; i < n; i++)
        a[i] = (double)rand() / ((double)RAND_MAX + 1.0);
    run_case("균등분포 무작위", a, n);
    free(a);
}

/* 대부분이 좁은 구간에 몰리고 이상치 하나 — 전제가 깨진 경우.
   정답은 여전히 나와야 한다. 느려질 뿐이다. */
static void case_clustered(int n)
{
    double *a = malloc((size_t)n * sizeof *a);
    if (!a) { fprintf(stderr, "OOM\n"); exit(1); }
    for (int i = 0; i < n; i++)
        a[i] = 1.0 + 0.001 * ((double)rand() / ((double)RAND_MAX + 1.0));
    a[n - 1] = 1000.0;                       /* 이상치 */
    run_case("치우친 분포 + 이상치", a, n);
    free(a);
}

/* 값 종류가 적어 중복이 많은 경우 */
static void case_many_duplicates(int n)
{
    double *a = malloc((size_t)n * sizeof *a);
    if (!a) { fprintf(stderr, "OOM\n"); exit(1); }
    for (int i = 0; i < n; i++)
        a[i] = (double)(rand() % 5);         /* 0,1,2,3,4 만 등장 */
    run_case("중복 다수 (값 5종)", a, n);
    free(a);
}

/* ---------- main ---------- */

int main(void)
{
    srand(20260902);

    printf("=== bucket_sort 검증 ===\n\n");

    printf("-- 기본 --\n");
    { double a[] = {0.1, 0.5, 0.2, 0.9, 0.46, 0.6};
      run_case("교재형 [0,1) 실수", a, 6); }
    { double a[] = {2, 8, 7, 1, 3, 5, 6, 4};
      run_case("무작위 정수값", a, 8); }

    printf("\n-- 순서 --\n");
    { double a[] = {1, 2, 3, 4, 5};
      run_case("이미 정렬됨", a, 5); }
    { double a[] = {5, 4, 3, 2, 1};
      run_case("역순", a, 5); }

    printf("\n-- 경계 --\n");
    run_case("빈 배열", NULL, 0);
    { double a[] = {7};
      run_case("원소 하나", a, 1); }
    { double a[] = {2, 1};
      run_case("원소 둘 (역순)", a, 2); }
    { double a[] = {1, 2};
      run_case("원소 둘 (정순)", a, 2); }
    { double a[] = {3.5, 3.5, 3.5, 3.5};
      run_case("전부 같은 값", a, 4); }
    { double a[] = {0.0, 1.0};
      run_case("최솟값이 0", a, 2); }

    printf("\n-- 버킷 분포 --\n");
    { double a[] = {0.05, 0.06, 0.07, 0.08, 0.95};
      run_case("한 버킷에 4개", a, 5); }
    { double a[] = {1, 1, 1, 2, 2, 3};
      run_case("중복 + 최댓값 경계", a, 6); }

    printf("\n-- 부동소수점 --\n");
    { double a[] = {1000000.3, 1000000.1, 1000000.2};
      run_case("큰 값 + 좁은 범위", a, 3); }
    { double a[] = {1e-9, 3e-9, 2e-9};
      run_case("아주 작은 값", a, 3); }

    printf("\n-- 대량 --\n");
    case_uniform(1000);
    case_uniform(10000);
    case_clustered(1000);
    case_many_duplicates(1000);

    printf("\n-- 전제 위반 (참고용) --\n");
    printf("     함수 주석이 '양의 실수'를 전제한다. 센티널 -1과 충돌하는지 확인.\n");
    { double a[] = {-3.0, -1.0, -2.0};
      run_case("음수 포함", a, 3); }

    printf("\n=== %d / %d PASS ===\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
