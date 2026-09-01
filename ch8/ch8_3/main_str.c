#include "radix_sort.h"
#include "counting_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 문자열 radix sort 검증.
   정수 버전과 별개 프로그램이라 main이 충돌하지 않는다.

   gcc -g -O0 -Wall -fsanitize=address counting_sort.c radix_sort.c main_str.c \
       -o ../../bin/radix_sort_str && ../../bin/radix_sort_str          */

/* 센티널 — 입력에 절대 안 나올 주소. 함수가 안 건드린 칸을 드러낸다.
   정수 버전의 -999에 해당. 값이 아니라 주소로 비교한다. */
static const char SENT_OBJ[] = "<<UNWRITTEN>>";
#define SENTINEL (SENT_OBJ)

static int g_pass = 0, g_fail = 0;

/* ── 검증 헬퍼 ───────────────────────────────────────────── */

static void print_words(const char *label, const char **a, size_t n)
{
    printf("%-8s", label);
    for (size_t i = 0; i < n; i++)
        printf(" %s", a[i] == SENTINEL ? "?" : a[i]);
    printf("\n");
}

static int all_written(const char **a, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (a[i] == SENTINEL) return 0;
    return 1;
}

static int is_sorted_str(const char **a, size_t n)
{
    for (size_t i = 0; i + 1 < n; i++)
        if (strcmp(a[i], a[i+1]) > 0) return 0;
    return 1;
}

/* 내용 일치 (strcmp 기준) */
static int equals_expected(const char **got, const char **want, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (strcmp(got[i], want[i]) != 0) return 0;
    return 1;
}

/* 출력이 입력의 순열인가 — 포인터 정체성으로 확인.
   radix sort는 포인터를 옮기기만 해야 한다. 문자열을 새로 만들거나
   한쪽 버퍼를 덮어쓰면 여기서 잡힌다.
   내용이 같아도 다른 객체면 다른 것으로 센다. */
static int is_permutation_ptr(const char **out, const char **in, size_t n)
{
    int *used = calloc(n > 0 ? n : 1, sizeof(int));
    if (used == NULL) return 0;

    for (size_t i = 0; i < n; i++)
    {
        size_t j;
        for (j = 0; j < n; j++)
            if (!used[j] && out[i] == in[j]) { used[j] = 1; break; }
        if (j == n) { free(used); return 0; }
    }
    free(used);
    return 1;
}

/* ── 전체 정렬 테스트 ─────────────────────────────────────── */

static void run_test(const char *name, const char **input, size_t n,
                     const char **expected)
{
    const char **result = malloc((n > 0 ? n : 1) * sizeof(*result));
    if (result == NULL) { printf("[%-14s] ALLOC FAIL\n", name); g_fail++; return; }
    for (size_t i = 0; i < n; i++) result[i] = SENTINEL;

    int rc = radix_sort_char(input, n, result);
    if (rc != 0)
    {
        printf("[%-14s] RETURN %d\n", name, rc);
        g_fail++; free(result); return;
    }

    int w = all_written(result, n);
    int e = w && equals_expected(result, expected, n);
    int s = w && is_sorted_str(result, n);
    int p = is_permutation_ptr(result, input, n);

    if (w && e && s && p)
    {
        printf("[%-14s] PASS ", name);
        for (size_t i = 0; i < n; i++) printf(" %s", result[i]);
        printf("\n");
        g_pass++;
    }
    else
    {
        printf("[%-14s] FAIL  written=%d equal=%d sorted=%d perm=%d\n",
               name, w, e, s, p);
        print_words("  got", result, n);
        print_words("  want", expected, n);
        g_fail++;
    }
    free(result);
}

/* ── 안정성 테스트 ────────────────────────────────────────────
   counting_sort_char 한 회차만 돌린다.
   같은 글자를 가진 원소들의 입력 순서가 유지되는지 포인터 정체성으로 확인.
   expected_idx: 기대하는 출력 순서를 input의 인덱스로 적은 것.

   8.2부터 미뤄온 검증이다. int 배열로는 같은 값을 구별할 수 없어 불가능했지만,
   문자열은 "키가 같고 내용이 다른" 원소를 만들 수 있어서 가능해졌다.       */

static void run_stability_test(const char *name, const char **input, size_t n,
                               int pos, const int *expected_idx)
{
    const char **result = malloc(n * sizeof(*result));
    if (result == NULL) { printf("[%-14s] ALLOC FAIL\n", name); g_fail++; return; }
    for (size_t i = 0; i < n; i++) result[i] = SENTINEL;

    int rc = counting_sort_char(input, n, result, 25, pos);
    if (rc != 0)
    {
        printf("[%-14s] RETURN %d\n", name, rc);
        g_fail++; free(result); return;
    }

    int ok = all_written(result, n);
    for (size_t i = 0; ok && i < n; i++)
        if (result[i] != input[expected_idx[i]]) ok = 0;   /* 주소 비교 */

    if (ok)
    {
        printf("[%-14s] PASS ", name);
        for (size_t i = 0; i < n; i++) printf(" %s", result[i]);
        printf("\n");
        g_pass++;
    }
    else
    {
        printf("[%-14s] FAIL  (pos=%d)\n", name, pos);
        print_words("  got", result, n);
        printf("  want   ");
        for (size_t i = 0; i < n; i++) printf(" %s", input[expected_idx[i]]);
        printf("\n");
        g_fail++;
    }
    free(result);
}

int main(void)
{
    /* ── 전체 정렬 ── */

    /* CLRS 8.3-1. 3글자 16개 → num=3 (홀수 경로) */
    const char *a1[] = {"COW","DOG","SEA","RUG","ROW","MOB","BOX","TAB",
                        "BAR","EAR","TAR","DIG","BIG","TEA","NOW","FOX"};
    const char *e1[] = {"BAR","BIG","BOX","COW","DIG","DOG","EAR","FOX",
                        "MOB","NOW","ROW","RUG","SEA","TAB","TAR","TEA"};
    run_test("1 CLRS 8.3-1", a1, 16, e1);

    /* num=1 */
    const char *a2[] = {"C","A","B"};
    const char *e2[] = {"A","B","C"};
    run_test("2 num=1", a2, 3, e2);

    /* num=2 — 짝수 회차. 핑퐁이 어긋나면 여기서 잡힌다 */
    const char *a3[] = {"BA","AB","AA","BB"};
    const char *e3[] = {"AA","AB","BA","BB"};
    run_test("3 num=2", a3, 4, e3);

    /* num=4 — 짝수 */
    const char *a4[] = {"DCBA","ABCD","AAAA","ZZZZ"};
    const char *e4[] = {"AAAA","ABCD","DCBA","ZZZZ"};
    run_test("4 num=4", a4, 4, e4);

    /* 1의 자리만 정렬하면 우연히 맞아 보이는 배열.
       ZA < ZB 는 뒷글자로 갈리지만 AB 가 앞으로 와야 한다 */
    const char *a5[] = {"ZA","ZB","AB","AA"};
    const char *e5[] = {"AA","AB","ZA","ZB"};
    run_test("5 첫글자 결정", a5, 4, e5);

    /* 경계 */

    const char *a6[] = {"XYZ"};
    const char *e6[] = {"XYZ"};
    run_test("6 n=1", a6, 1, e6);

    const char *a7[] = {"XYZ"};          /* n=0 이라 내용은 안 읽힌다 */
    run_test("7 n=0", a7, 0, a7);

    const char *a8[] = {"AAA","AAA","AAA"};
    const char *e8[] = {"AAA","AAA","AAA"};
    run_test("8 all same", a8, 3, e8);

    const char *a9[] = {"ABC","ABD","ABE"};
    const char *e9[] = {"ABC","ABD","ABE"};
    run_test("9 sorted", a9, 3, e9);

    const char *a10[] = {"CBA","BBA","ABA"};
    const char *e10[] = {"ABA","BBA","CBA"};
    run_test("10 reverse", a10, 3, e10);

    const char *a11[] = {"AB","AB","AA"};
    const char *e11[] = {"AA","AB","AB"};
    run_test("11 dup", a11, 3, e11);

    /* 알파벳 양 끝. k=25 가 틀리면 Z 에서 힙 오버플로 */
    const char *a12[] = {"ZZZ","AAA","ZAZ","AZA"};
    const char *e12[] = {"AAA","AZA","ZAZ","ZZZ"};
    run_test("12 A~Z 경계", a12, 4, e12);

    /* ── 안정성 (counting_sort_char 단독) ── */
    printf("\n");

    /* pos=0 기준. B 셋(BAT,BOX,BAR)과 A 둘(APE,AXE)의 입력 순서가 유지돼야 한다.
       역방향 순회가 정방향으로 바뀌면 각 그룹 안이 뒤집힌다. */
    const char *s1[] = {"BAT","BOX","BAR","APE","AXE"};
    const int   x1[] = {3, 4, 0, 1, 2};        /* APE AXE BAT BOX BAR */
    run_stability_test("13 stable p0", s1, 5, 0, x1);

    /* pos=2 기준. 끝글자 E:{APE,AXE}, R:{BAR}, T:{BAT}, X:{BOX}
       E 가 둘이므로 입력 순서(APE 먼저)가 유지돼야 한다 */
    const int   x2[] = {3, 4, 2, 0, 1};        /* APE AXE BAR BAT BOX */
    run_stability_test("14 stable p2", s1, 5, 2, x2);

    /* 전부 같은 키. 입력 순서가 그대로 나와야 한다 */
    const char *s3[] = {"AX","AY","AZ","AW"};
    const int   x3[] = {0, 1, 2, 3};
    run_stability_test("15 stable same", s3, 4, 0, x3);

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
