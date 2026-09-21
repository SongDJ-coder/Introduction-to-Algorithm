/* ch10_2/main.c — 연결 리스트 테스트 하네스 (Claude 작성)
 *
 * 검사 5중:
 *   [a] next 방향 순회가 기준 모델과 일치
 *   [b] prev 방향 순회가 기준 모델의 역순과 일치   <- prev 사슬이 끊겨도 잡는다
 *   [c] 순환 불변식  nil.next->prev == &nil,  nil.prev->next == &nil
 *   [d] 길이 일치
 *   [e] 반환값 (성공/실패)
 *
 * 기준 정답은 배열 모델. 삽입은 맨 앞, 삭제는 "첫 번째로 만난 값" 규약.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Linkedlist.h"

#define MAXN 512

static int pass_count = 0;
static int fail_count = 0;

/* ---------------- 기준 모델 (배열) ---------------- */

typedef struct {
    int a[MAXN];
    int n;
} Model;

static void model_init(Model *m) { m->n = 0; }

static void model_insert(Model *m, int data)
{
    for (int i = m->n; i > 0; i--) m->a[i] = m->a[i - 1];
    m->a[0] = data;
    m->n++;
}

/* 0 성공 / 1 없음 — Delete_Data 와 같은 규약 */
static int model_delete(Model *m, int data)
{
    int idx = -1;
    for (int i = 0; i < m->n; i++) {
        if (m->a[i] == data) { idx = i; break; }
    }
    if (idx < 0) return 1;
    for (int i = idx; i < m->n - 1; i++) m->a[i] = m->a[i + 1];
    m->n--;
    return 0;
}

static int model_has(const Model *m, int data)
{
    for (int i = 0; i < m->n; i++) if (m->a[i] == data) return 1;
    return 0;
}

/* ---------------- 검증 ---------------- */

static void report(const char *what, int ok, const char *detail)
{
    if (ok) {
        pass_count++;
    } else {
        fail_count++;
        printf("  FAIL  %-34s %s\n", what, detail ? detail : "");
    }
}

/* 리스트 상태가 모델과 일치하는지 5중 검사 */
static void check_state(List *L, const Model *m, const char *what)
{
    Node *nil = &L->nil;
    char detail[256];

    /* [c] 순환 불변식 */
    if (nil->next == NULL || nil->prev == NULL) {
        report(what, 0, "nil.next 또는 nil.prev 가 NULL (순환이 깨짐)");
        return;
    }
    if (nil->next->prev != nil) {
        report(what, 0, "nil.next->prev != &nil");
        return;
    }
    if (nil->prev->next != nil) {
        report(what, 0, "nil.prev->next != &nil");
        return;
    }

    /* [a][d] next 방향 */
    int fwd[MAXN + 8];
    int fn = 0;
    for (Node *x = nil->next; x != nil; x = x->next) {
        if (fn > MAXN + 4) { report(what, 0, "next 순회가 안 끝난다 (순환 깨짐/무한루프)"); return; }
        fwd[fn++] = x->data;
    }
    if (fn != m->n) {
        snprintf(detail, sizeof detail, "길이 next=%d 기대=%d", fn, m->n);
        report(what, 0, detail);
        return;
    }
    for (int i = 0; i < fn; i++) {
        if (fwd[i] != m->a[i]) {
            snprintf(detail, sizeof detail, "next[%d]=%d 기대=%d", i, fwd[i], m->a[i]);
            report(what, 0, detail);
            return;
        }
    }

    /* [b] prev 방향 — 모델의 역순이어야 한다 */
    int bwd[MAXN + 8];
    int bn = 0;
    for (Node *x = nil->prev; x != nil; x = x->prev) {
        if (bn > MAXN + 4) { report(what, 0, "prev 순회가 안 끝난다"); return; }
        bwd[bn++] = x->data;
    }
    if (bn != m->n) {
        snprintf(detail, sizeof detail, "길이 prev=%d 기대=%d", bn, m->n);
        report(what, 0, detail);
        return;
    }
    for (int i = 0; i < bn; i++) {
        if (bwd[i] != m->a[m->n - 1 - i]) {
            snprintf(detail, sizeof detail, "prev[%d]=%d 기대=%d", i, bwd[i], m->a[m->n - 1 - i]);
            report(what, 0, detail);
            return;
        }
    }

    report(what, 1, NULL);
}

/* Search_Data 가 모델과 일치하는지 */
static void check_search(List *L, const Model *m, int data, const char *what)
{
    Node *got = Search_Data(data, L);
    int want = model_has(m, data);
    char detail[256];

    if (want && got == NULL) {
        snprintf(detail, sizeof detail, "값 %d 가 있는데 NULL 반환", data);
        report(what, 0, detail);
    } else if (!want && got != NULL) {
        snprintf(detail, sizeof detail, "값 %d 가 없는데 %p 반환", data, (void *)got);
        report(what, 0, detail);
    } else if (want && got->data != data) {
        snprintf(detail, sizeof detail, "값 %d 를 찾았는데 data=%d", data, got->data);
        report(what, 0, detail);
    } else if (want && got == &L->nil) {
        report(what, 0, "보초를 반환했다");
    } else {
        report(what, 1, NULL);
    }
}

/* 헬퍼 — 리스트와 모델에 동시에 적용 */
static void both_insert(List *L, Model *m, int data)
{
    int r = Insert_Data(data, L);
    if (r != 0) { report("Insert_Data 반환값", 0, "0 이 아닌 값 (malloc 실패?)"); return; }
    model_insert(m, data);
}

static void both_delete(List *L, Model *m, int data, const char *what)
{
    int got = Delete_Data(data, L);
    int want = model_delete(m, data);
    char detail[128];
    if (got != want) {
        snprintf(detail, sizeof detail, "값 %d — 반환 %d, 기대 %d", data, got, want);
        report(what, 0, detail);
    } else {
        report(what, 1, NULL);
    }
}

/* ---------------- 케이스 ---------------- */

static void case1_empty(void)
{
    printf("[1] 빈 리스트\n");
    List L;
    Model m;
    Setup_List(&L);
    model_init(&m);

    check_state(&L, &m, "setup 직후 상태");
    check_search(&L, &m, 42, "빈 리스트 search -> NULL");
    both_delete(&L, &m, 42, "빈 리스트 delete -> 1");
    check_state(&L, &m, "실패한 delete 후에도 무결");

    All_Delete(&L);
    check_state(&L, &m, "빈 리스트에 All_Delete");
}

static void case2_single(void)
{
    printf("[2] 원소 1개\n");
    List L;
    Model m;
    Setup_List(&L);
    model_init(&m);

    both_insert(&L, &m, 7);
    check_state(&L, &m, "1개 삽입");
    check_search(&L, &m, 7, "있는 값 search");
    check_search(&L, &m, 8, "없는 값 search");

    both_delete(&L, &m, 7, "유일한 원소 delete");
    check_state(&L, &m, "유일한 원소 삭제 후 빈 리스트");

    /* 삭제 후 재삽입 */
    both_insert(&L, &m, 99);
    check_state(&L, &m, "삭제 후 재삽입");
    All_Delete(&L);
}

static void case3_positions(void)
{
    printf("[3] head / tail / 중간 삭제\n");
    int vals[] = {1, 2, 3, 4, 5};

    /* 삽입은 앞이므로 리스트는 5 4 3 2 1 — head=5, tail=1 */
    struct { int target; const char *name; } t[] = {
        {5, "head 삭제"},
        {1, "tail 삭제"},
        {3, "중간 삭제"},
        {9, "없는 값 삭제 -> 1"},
    };

    for (size_t k = 0; k < sizeof t / sizeof t[0]; k++) {
        List L;
        Model m;
        Setup_List(&L);
        model_init(&m);
        for (size_t i = 0; i < sizeof vals / sizeof vals[0]; i++)
            both_insert(&L, &m, vals[i]);

        both_delete(&L, &m, t[k].target, t[k].name);
        check_state(&L, &m, t[k].name);
        All_Delete(&L);
    }
}

static void case4_duplicates(void)
{
    printf("[4] 중복 값 — 첫 번째만 삭제\n");
    List L;
    Model m;
    Setup_List(&L);
    model_init(&m);

    /* 삽입 순서 5,5,5 -> 리스트 5 5 5 */
    both_insert(&L, &m, 5);
    both_insert(&L, &m, 5);
    both_insert(&L, &m, 5);
    check_state(&L, &m, "같은 값 3개 삽입");

    both_delete(&L, &m, 5, "중복 1회 삭제");
    check_state(&L, &m, "중복 1개 삭제 후 2개 남음");
    both_delete(&L, &m, 5, "중복 2회 삭제");
    both_delete(&L, &m, 5, "중복 3회 삭제");
    check_state(&L, &m, "중복 전부 삭제 후 빈 리스트");
    both_delete(&L, &m, 5, "네 번째 삭제 -> 1");

    All_Delete(&L);
}

static void case5_alldelete_reuse(void)
{
    printf("[5] All_Delete 후 재사용\n");
    List L;
    Model m;
    Setup_List(&L);
    model_init(&m);

    for (int i = 0; i < 10; i++) both_insert(&L, &m, i);
    check_state(&L, &m, "10개 삽입");

    All_Delete(&L);
    model_init(&m);
    check_state(&L, &m, "All_Delete 후 빈 리스트");
    check_search(&L, &m, 3, "All_Delete 후 search -> NULL");

    for (int i = 100; i < 105; i++) both_insert(&L, &m, i);
    check_state(&L, &m, "All_Delete 후 재삽입 5개");
    both_delete(&L, &m, 102, "재삽입한 값 삭제");
    check_state(&L, &m, "재삽입 후 삭제");

    All_Delete(&L);
}

static void case6_extremes(void)
{
    printf("[6] 극단값\n");
    List L;
    Model m;
    Setup_List(&L);
    model_init(&m);

    int vals[] = {0, -1, 2147483647, -2147483647 - 1};
    for (size_t i = 0; i < sizeof vals / sizeof vals[0]; i++)
        both_insert(&L, &m, vals[i]);
    check_state(&L, &m, "INT_MIN/INT_MAX/0/-1 삽입");

    for (size_t i = 0; i < sizeof vals / sizeof vals[0]; i++)
        check_search(&L, &m, vals[i], "극단값 search");

    both_delete(&L, &m, 0, "0 삭제");
    check_state(&L, &m, "0 삭제 후");

    All_Delete(&L);
}

static void case7_stress(unsigned seed)
{
    printf("[7] 무작위 스트레스 (seed=%u)\n", seed);
    srand(seed);

    for (int round = 0; round < 200; round++) {
        List L;
        Model m;
        Setup_List(&L);
        model_init(&m);

        int ops = 20 + rand() % 60;
        int spread = (round % 3 == 0) ? 5 : (round % 3 == 1) ? 30 : 200;

        for (int i = 0; i < ops; i++) {
            int v = rand() % spread;
            int coin = rand() % 100;

            if (coin < 55 || m.n == 0) {
                if (m.n >= MAXN - 2) continue;
                if (Insert_Data(v, &L) != 0) { report("stress insert", 0, "삽입 실패"); break; }
                model_insert(&m, v);
            } else if (coin < 90) {
                int got = Delete_Data(v, &L);
                int want = model_delete(&m, v);
                if (got != want) { report("stress delete 반환값", 0, "불일치"); break; }
            } else {
                Node *got = Search_Data(v, &L);
                int want = model_has(&m, v);
                if ((got != NULL) != (want != 0)) { report("stress search", 0, "불일치"); break; }
            }
        }

        check_state(&L, &m, "stress 라운드 최종 상태");
        All_Delete(&L);
        model_init(&m);
        check_state(&L, &m, "stress All_Delete 후");
    }
}

int main(void)
{
    unsigned seed = (unsigned)time(NULL);
    /* 재현이 필요하면 아래 줄의 주석을 풀고 실패했던 seed 를 박는다 */
    /* seed = 1234567890u; */

    printf("=== ch10.2 Linked list 하네스 ===\n\n");

    case1_empty();
    case2_single();
    case3_positions();
    case4_duplicates();
    case5_alldelete_reuse();
    case6_extremes();
    case7_stress(seed);

    printf("\n=== %d / %d PASS", pass_count, pass_count + fail_count);
    if (fail_count) printf("  (FAIL %d)  seed=%u", fail_count, seed);
    printf(" ===\n");

    return fail_count ? 1 : 0;
}
