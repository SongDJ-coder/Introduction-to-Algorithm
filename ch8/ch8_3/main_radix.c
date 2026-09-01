#include "radix_sort.h"
#include "counting_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* radix 일반화 검증 + 성능 측정.
 *
 * 정확성 (ASan 켬):
 *   gcc -g -O0 -Wall -fsanitize=address counting_sort.c radix_sort.c main_radix.c \
 *       -o ../../bin/radix_perf && ../../bin/radix_perf
 *
 * 시간 측정 (ASan 끔 — 켜면 2배 느려져서 의미 없음):
 *   gcc -g -O2 -Wall counting_sort.c radix_sort.c main_radix.c \
 *       -o ../../bin/radix_perf && ../../bin/radix_perf
 */

#define SENTINEL (-999)

static int g_pass = 0, g_fail = 0;

/* ── 검증 헬퍼 ───────────────────────────────────────────── */

static int all_written(const int *a, size_t n)
{
    for (size_t i = 0; i < n; i++) if (a[i] == SENTINEL) return 0;
    return 1;
}

static int is_sorted(const int *a, size_t n)
{
    for (size_t i = 0; i + 1 < n; i++) if (a[i] > a[i+1]) return 0;
    return 1;
}

static int arrays_equal(const int *a, const int *b, size_t n)
{
    for (size_t i = 0; i < n; i++) if (a[i] != b[i]) return 0;
    return 1;
}

static int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);          /* 뺄셈은 오버플로 위험 */
}

/* 기준 정답을 qsort로 만든다. radix sort와 독립적인 구현이라
   양쪽이 같은 버그를 공유할 수 없다. */
static int *reference_sorted(const int *in, size_t n)
{
    int *ref = malloc((n > 0 ? n : 1) * sizeof(int));
    if (ref == NULL) return NULL;
    for (size_t i = 0; i < n; i++) ref[i] = in[i];
    qsort(ref, n, sizeof(int), cmp_int);
    return ref;
}

/* ── 정확성 ──────────────────────────────────────────────── */

static void run_test(const char *name, const int *input, size_t n, int radix)
{
    int *ref = reference_sorted(input, n);
    int *result = malloc((n > 0 ? n : 1) * sizeof(int));
    if (ref == NULL || result == NULL)
    {
        printf("[%-20s] ALLOC FAIL\n", name);
        g_fail++; free(ref); free(result); return;
    }
    for (size_t i = 0; i < n; i++) result[i] = SENTINEL;

    int rc = radix_sort_r(input, n, result, radix);
    if (rc != 0)
    {
        printf("[%-20s] RETURN %d\n", name, rc);
        g_fail++; free(ref); free(result); return;
    }

    int w = all_written(result, n);
    int e = w && arrays_equal(result, ref, n);
    int s = w && is_sorted(result, n);

    if (w && e && s)
    {
        printf("[%-20s] PASS\n", name);
        g_pass++;
    }
    else
    {
        printf("[%-20s] FAIL  written=%d equal=%d sorted=%d\n", name, w, e, s);
        if (n <= 20)
        {
            printf("  got  ");
            for (size_t i = 0; i < n; i++) printf(" %d", result[i]);
            printf("\n  want ");
            for (size_t i = 0; i < n; i++) printf(" %d", ref[i]);
            printf("\n");
        }
        g_fail++;
    }
    free(ref); free(result);
}

/* 같은 입력을 여러 radix로 돌려서 전부 같은 답이 나오는지.
   radix는 구현 세부사항이므로 결과에 영향을 주면 안 된다. */
static void run_all_radices(const char *name, const int *input, size_t n)
{
    static const int radices[] = {2, 8, 10, 16, 100, 256, 1024, 65536};
    char label[64];
    for (size_t r = 0; r < sizeof(radices)/sizeof(radices[0]); r++)
    {
        snprintf(label, sizeof(label), "%s r=%d", name, radices[r]);
        run_test(label, input, n, radices[r]);
    }
}

/* ── 성능 ────────────────────────────────────────────────── */

static double measure(const int *input, size_t n, int radix)
{
    int *result = malloc(n * sizeof(int));
    if (result == NULL) return -1.0;

    clock_t t0 = clock();
    int rc = radix_sort_r(input, n, result, radix);
    double sec = (double)(clock() - t0) / CLOCKS_PER_SEC;

    if (rc != 0 || !is_sorted(result, n)) sec = -1.0;
    free(result);
    return sec;
}

/* max 를 radix 진법으로 쓰면 몇 자리인가 = 회차 수 */
static int digit_count(int max, int radix)
{
    long long k = 1;
    int d = 0;
    while (max / k != 0) { k *= radix; d++; }
    return d == 0 ? 1 : d;
}

static void perf_table(const char *title, const int *input, size_t n, int max)
{
    /* 뒤쪽 넷은 32비트 범위에서 전부 d=2 가 된다.
       d 를 고정하고 k 만 키워서 k 항이 언제 보이기 시작하는지 찾는다. */
    static const int radices[] = {2, 10, 16, 256, 1024, 65536,
                                  262144, 1048576, 4194304, 16777216};

    printf("\n=== %s  (n = %zu, max = %d) ===\n", title, n, max);
    printf("%10s %6s %12s %10s %10s %10s\n",
           "radix", "d", "배열(KB)", "시간(초)", "회차당", "비고");

    for (size_t r = 0; r < sizeof(radices)/sizeof(radices[0]); r++)
    {
        int radix = radices[r];
        int d = digit_count(max, radix);
        double kb = (double)radix * sizeof(int) / 1024.0;
        double sec = measure(input, n, radix);

        /* 캐시 계층 어디에 들어가는지 대략 표시 (전형적인 x86 기준) */
        const char *note = kb <= 32     ? "L1"
                         : kb <= 1024   ? "L2"
                         : kb <= 32768  ? "L3"
                         :                "RAM";

        if (sec < 0) printf("%10d %6d %12.0f %10s %10s %10s\n",
                            radix, d, kb, "FAIL", "-", note);
        else         printf("%10d %6d %12.0f %10.4f %10.4f %10s\n",
                            radix, d, kb, sec, sec / d, note);
    }
}

/* ── 데이터 생성 ─────────────────────────────────────────── */

static int *make_random(size_t n, int max, int *out_actual_max)
{
    int *a = malloc(n * sizeof(int));
    if (a == NULL) return NULL;
    int m = 0;
    for (size_t i = 0; i < n; i++)
    {
        /* rand()는 0..RAND_MAX(보통 2^31-1). max 범위로 줄인다 */
        int v = (int)(((long long)rand() * max) / RAND_MAX);
        a[i] = v;
        if (v > m) m = v;
    }
    *out_actual_max = m;
    return a;
}

int main(void)
{
    srand(42);          /* 고정 시드 — 재현 가능하게 */

    /* ── 1단계: 정확성. radix가 달라도 답은 같아야 한다 ── */

    printf("=== 정확성 ===\n");

    int a1[] = {3, 2, 5, 7, 4, 3, 1, 8, 0, 6, 3, 10, 19};
    run_all_radices("mixed", a1, 13);

    int a2[] = {0, 0, 0, 0};
    run_all_radices("zeros", a2, 4);

    int a3[] = {7};
    run_all_radices("single", a3, 1);

    int a4[] = {2147483647, 0, 1, 2147483646};
    run_all_radices("INT_MAX", a4, 4);

    int a5[] = {1, 255, 256, 257, 65535, 65536, 65537};
    run_all_radices("radix 경계", a5, 7);

    /* 무작위 대량 — 손으로 만든 케이스가 놓치는 걸 잡는다 */
    int rmax;
    int *big = make_random(10000, 1000000, &rmax);
    if (big != NULL) { run_all_radices("random 10k", big, 10000); free(big); }

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    if (g_fail > 0)
    {
        printf("\n정확성이 깨졌으므로 성능 측정을 건너뜁니다.\n");
        return 1;
    }

    /* ── 2단계: 성능 ── */

    size_t n = 1000000;

    int m1;
    int *d1 = make_random(n, 2147483647, &m1);
    if (d1 != NULL) { perf_table("32비트 전 범위", d1, n, m1); free(d1); }

    int m2;
    int *d2 = make_random(n, 999, &m2);
    if (d2 != NULL) { perf_table("작은 값 (0~999)", d2, n, m2); free(d2); }

    printf("\n읽는 법: 회차 d 가 줄면 빨라지지만 k+1 칸이 n 에 가까워지면\n");
    printf("         alphabet 배열 초기화·누적합 비용이 커져 다시 느려진다.\n");
    printf("         CLRS 정리 8.4: Theta((b/r)(n + 2^r)), 최적 r ~ lg n\n");

    return 0;
}
