/* ch9_2/main_cost.c — 비용 검증 하네스
 *
 * main.c 가 "답이 맞는가"를 봤다면 여기는 "정말 Θ(n) 인가"를 본다.
 * 정확성 테스트만으로는 qsort 후 [i-1] 을 돌려주는 Θ(n lg n) 구현과
 * 구별되지 않는다. (9.1 에서 15/15 가 전부 가짜였던 것과 같은 구멍)
 *
 * 방법 — 절대량이 아니라 **비율**을 본다
 *   9.2 는 비교 횟수가 난수에 따라 변하므로 9.1 식 정확 일치 검사를 쓸 수 없다.
 *   대신 n 을 2배씩 키우며 평균 비교 횟수를 재고
 *     ① C(n)/n 이 상수로 수렴하는가        → Θ(n)
 *     ② C(2n)/C(n) 이 2.0 에 가까운가       → Θ(n)
 *   를 본다. 상수를 몰라도 되는 것이 이 방법의 장점이다.
 *
 * 대조군 — 같은 partition 을 쓰는 randomized_quicksort
 *   계측 카운터가 partition 안에 있으므로 quicksort 도 같은 자로 잴 수 있다.
 *   둘의 차이는 오직 "한쪽만 재귀하는가 / 양쪽 다 하는가" 뿐이다.
 *   quicksort 의 C(n)/n 은 ~1.39 lg n 이라 n 이 커질수록 **자란다.**
 *   select 는 평평해야 한다. 이 대비가 Θ(n) 주장의 증거다.
 *
 * 빌드 (ASan 끄고 -O2. 비교 횟수는 ASan 과 무관하지만 훨씬 빠르다)
 *   gcc -O2 -Wall -Wextra ../../ch7/quicksort.c randomized_select.c main_cost.c \
 *       -o ../../bin/select_cost && ../../bin/select_cost
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../ch7/quicksort.h"
#include "randomized_select.h"

#define TRIALS 40          /* n 하나당 반복 횟수 */
#define N_MIN  1024
#define N_MAX  262144      /* 1K -> 256K, 9단계 */

/* i 를 고르는 세 가지 전략 — 순위에 따라 비용이 달라지는지 본다 */
enum { PICK_FIRST, PICK_MID, PICK_LAST, PICK_COUNT };

static const char *pick_name[PICK_COUNT] = { "i=1", "i=n/2", "i=n" };

static int pick_rank(int which, int n)
{
    switch (which) {
    case PICK_FIRST: return 1;
    case PICK_MID:   return (n + 1) / 2;
    default:         return n;
    }
}

static int *make_array(int n)
{
    int *a = malloc((size_t)n * sizeof *a);
    int j;

    if (a == NULL) {
        fprintf(stderr, "malloc failed (n = %d)\n", n);
        exit(EXIT_FAILURE);
    }
    for (j = 0; j < n; j++)
        a[j] = rand();
    return a;
}

int main(void)
{
    unsigned seed = (unsigned)time(NULL);
    int n, t, which;

    /* 이전 n 의 평균을 들고 있다가 비율을 낸다 */
    double prev_sel[PICK_COUNT];
    double prev_qs = 0.0;
    int have_prev = 0;

    srand(seed);   /* 프로그램 전체에서 딱 한 번 */

    memset(prev_sel, 0, sizeof prev_sel);

    printf("=== RANDOMIZED-SELECT 비용 검증 ===\n");
    printf("seed = %u,  각 n 마다 %d회 평균\n\n", seed, TRIALS);

    printf("주장: select 의 C(n)/n 은 상수로 수렴하고, C(2n)/C(n) 은 2.0 에 수렴한다.\n");
    printf("대조: quicksort 는 같은 partition 을 쓰지만 양쪽을 다 재귀하므로\n");
    printf("      C(n)/n 이 ~1.39 lg n 으로 **자란다**.\n\n");

    printf("%9s | %-24s | %-24s | %-24s | %-22s\n",
           "n", "select i=1  C/n (ratio)", "select i=n/2 C/n (rat.)",
           "select i=n  C/n (ratio)", "quicksort C/n (ratio)");
    printf("----------|--------------------------"
           "|--------------------------"
           "|--------------------------"
           "|-----------------------\n");

    for (n = N_MIN; n <= N_MAX; n *= 2) {

        double avg_sel[PICK_COUNT];
        double avg_qs = 0.0;

        /* ---- select: i 전략 3종 ---- */
        for (which = 0; which < PICK_COUNT; which++) {
            double total = 0.0;

            for (t = 0; t < TRIALS; t++) {
                int *input = make_array(n);
                int *work  = malloc((size_t)n * sizeof *work);
                int out = 0;

                if (work == NULL) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
                memcpy(work, input, (size_t)n * sizeof *work);

                reset_comparisons();
                if (randomized_select(work, (size_t)n, pick_rank(which, n), &out) != 0) {
                    fprintf(stderr, "randomized_select 가 실패 반환 (n=%d)\n", n);
                    exit(EXIT_FAILURE);
                }
                total += (double)get_comparisons();

                free(work);
                free(input);
            }
            avg_sel[which] = total / TRIALS;
        }

        /* ---- 대조군: 같은 partition 을 쓰는 randomized_quicksort ---- */
        {
            double total = 0.0;

            for (t = 0; t < TRIALS; t++) {
                int *work = make_array(n);

                reset_comparisons();
                randomized_quicksort(work, 0, n - 1);
                total += (double)get_comparisons();

                free(work);
            }
            avg_qs = total / TRIALS;
        }

        /* ---- 출력 ---- */
        printf("%9d |", n);
        for (which = 0; which < PICK_COUNT; which++) {
            printf(" %8.3f", avg_sel[which] / n);
            if (have_prev) printf(" (%5.2f)  |", avg_sel[which] / prev_sel[which]);
            else           printf("    (--)  |");
        }
        printf(" %8.3f", avg_qs / n);
        if (have_prev) printf(" (%5.2f)\n", avg_qs / prev_qs);
        else           printf("    (--)\n");
        fflush(stdout);

        for (which = 0; which < PICK_COUNT; which++)
            prev_sel[which] = avg_sel[which];
        prev_qs  = avg_qs;
        have_prev = 1;
    }

    printf("\n읽는 법\n");
    printf("  C/n     — 원소 하나당 비교 횟수. select 는 평평, quicksort 는 자라야 한다\n");
    printf("  (ratio) — n 을 2배 했을 때 총 비교 횟수의 배율\n");
    printf("            Θ(n)     -> 2.00\n");
    printf("            Θ(n lg n)-> 2 + 2/lg n  (n=1024 에서 2.20, n=262144 에서 2.11)\n");
    printf("\n  ratio 만으로는 둘이 잘 안 갈린다. **C/n 열이 평평한가 자라는가**가 결정적이다.\n");
    printf("  이론 구간: 낙관(매번 정중앙) 2n, 비관(항상 큰 쪽) 4n -> C/n 은 2~4 사이\n");

    return 0;
}
