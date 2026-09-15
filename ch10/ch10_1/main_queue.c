/* CLRS 10.1 — queue 테스트 하네스 (Claude 작성)
 *
 * 한 칸 버리기(방식 1) 링 버퍼를 검증한다.
 *   - FIFO 순서
 *   - capacity 칸에 capacity-1 개만 담긴다        <- 이 설계의 핵심 주장
 *   - 되감기 경계                                  <- 유일하게 조용히 틀리는 자리
 *   - underflow / overflow 거부
 *   - NULL 인자
 *   - 무작위 스트레스: 매 연산을 모델 큐와 대조
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"

static int g_pass = 0;
static int g_fail = 0;

static void check(int ok, const char *name, const char *detail)
{
    if (ok) { g_pass++; printf("  PASS  %-30s %s\n", name, detail); }
    else    { g_fail++; printf("  FAIL  %-30s %s\n", name, detail); }
}

/* 배열 전체를 찍되 큐에 실제로 들어 있는 칸만 [ ] 로 감싼다.
 * head 부터 tail 직전까지가 큐다 — 되감기 때문에 tail < head 일 수 있다. */
static void dump(const Queue *q, const char *label)
{
    int i, live;

    printf("  %-14s h=%d t=%d  ", label, q->head, q->tail);
    for (i = 0; i < q->capacity; i++) {
        if (q->head <= q->tail) live = (i >= q->head && i < q->tail);
        else                    live = (i >= q->head || i < q->tail);
        printf(live ? "[%3d]" : " %3d ", q->arr[i]);
    }
    if (q->head == q->tail) printf("   <- 비어 있음");
    if ((q->tail + 1) % q->capacity == q->head) printf("   <- 가득 참");
    printf("\n");
}

/* ------------------------------------------------------------------ */

static void test_init(void)
{
    Queue q;
    int   buf[6];
    char  detail[64];

    printf("\n[1] set_queue\n");

    check(set_queue(&q, buf, 6) == 0, "set_queue(&q, buf, 6)", "-> 0");
    snprintf(detail, sizeof detail, "cap=%d head=%d tail=%d", q.capacity, q.head, q.tail);
    check(q.capacity == 6 && q.head == 0 && q.tail == 0, "fields after init", detail);
    check(queue_empty(&q) == 1, "empty on fresh queue", "-> 1");

    check(set_queue(NULL, buf, 6) != 0, "set_queue(NULL, ...)", "rejected");
    check(set_queue(&q, NULL, 6) != 0, "set_queue(&q, NULL, 6)", "rejected");
    check(set_queue(&q, buf, 0)  != 0, "set_queue(&q, buf, 0)",  "rejected");
    check(set_queue(&q, buf, -1) != 0, "set_queue(&q, buf, -1)", "rejected");
}

static void test_textbook_10_1_3(void)
{
    /* 교재 연습문제 10.1-3
       ENQUEUE(4), ENQUEUE(1), ENQUEUE(3), DEQUEUE, ENQUEUE(8), DEQUEUE  on Q[1..6] */
    Queue q;
    int   buf[6] = { 0 };
    int   v;
    char  label[32];

    printf("\n[2] 연습문제 10.1-3 의 연산 순서\n");
    set_queue(&q, buf, 6);
    dump(&q, "(시작)");

    queue_push(&q, 4); dump(&q, "enqueue(4)");
    queue_push(&q, 1); dump(&q, "enqueue(1)");
    queue_push(&q, 3); dump(&q, "enqueue(3)");

    queue_pop(&q, &v);
    snprintf(label, sizeof label, "dequeue -> %d", v);
    dump(&q, label);
    check(v == 4, "first dequeue", "-> 4 (FIFO)");

    queue_push(&q, 8); dump(&q, "enqueue(8)");

    queue_pop(&q, &v);
    snprintf(label, sizeof label, "dequeue -> %d", v);
    dump(&q, label);
    check(v == 1, "second dequeue", "-> 1");
}

static void test_capacity(void)
{
    Queue q;
    int   buf[5] = { 0 };
    int   i, v, ok = 1;
    char  detail[64];

    printf("\n[3] capacity 5 -> 원소는 4개까지 (한 칸 버리기)\n");
    set_queue(&q, buf, 5);

    for (i = 1; i <= 4; i++)
        if (queue_push(&q, i * 11) != 0) ok = 0;
    check(ok, "enqueue 4 elements", "all accepted");
    dump(&q, "after 4");

    check(queue_push(&q, 99) != 0, "enqueue 5th element", "rejected (full)");
    snprintf(detail, sizeof detail, "h=%d t=%d unchanged", q.head, q.tail);
    check(q.head == 0 && q.tail == 4, "state after overflow", detail);

    /* 버려진 칸이 실제로 안 쓰였는지 — buf[4] 는 0 그대로여야 한다 */
    check(buf[4] == 0, "sacrificed slot untouched", "buf[4] == 0");

    ok = 1;
    for (i = 1; i <= 4; i++)
        if (queue_pop(&q, &v) != 0 || v != i * 11) ok = 0;
    check(ok, "dequeue order", "11,22,33,44 (FIFO)");
    check(queue_empty(&q) == 1, "empty after draining", "-> 1");
}

static void test_wraparound(void)
{
    Queue q;
    int   buf[4] = { 0 };
    int   i, v, ok = 1;

    printf("\n[4] 되감기 — cap=4 에서 30바퀴 돌린다\n");
    set_queue(&q, buf, 4);

    /* 항상 1~2개만 들고 계속 밀어넣으면 head/tail 이 배열을 여러 바퀴 돈다 */
    queue_push(&q, 0);
    for (i = 1; i <= 120; i++) {
        if (queue_push(&q, i) != 0)            { ok = 0; printf("    enqueue %d 실패\n", i); break; }
        if (queue_pop(&q, &v) != 0)            { ok = 0; printf("    dequeue 실패\n");      break; }
        if (v != i - 1)                        { ok = 0; printf("    i=%d: want %d got %d\n", i, i-1, v); break; }
    }
    check(ok, "120 wraps, FIFO preserved", "head/tail 이 30바퀴 돌아도 정확");
    dump(&q, "after wraps");
}

static void test_underflow(void)
{
    Queue q;
    int   buf[4];
    int   v = -777;

    printf("\n[5] underflow\n");
    set_queue(&q, buf, 4);

    check(queue_pop(&q, &v) != 0, "dequeue on empty queue", "rejected");
    check(v == -777, "result untouched on failure", "unchanged");

    queue_push(&q, 5);
    queue_pop(&q, &v);
    check(queue_pop(&q, &v) != 0, "dequeue after draining", "rejected");
}

static void test_null(void)
{
    Queue q;
    int   buf[4];
    int   v;

    printf("\n[6] NULL 인자\n");
    set_queue(&q, buf, 4);

    check(queue_push(NULL, 1) != 0, "enqueue(NULL, 1)", "rejected");
    check(queue_pop(NULL, &v) != 0, "dequeue(NULL, &v)", "rejected");
    check(queue_pop(&q, NULL) != 0, "dequeue(&q, NULL)", "rejected");
    check(queue_empty(NULL) == 1,   "empty(NULL)",       "-> 1");

    /* 정상 호출이 거부되지 않는지 — result 검사가 뒤집혔던 자리 */
    check(queue_push(&q, 7) == 0,   "enqueue(&q, 7)",    "accepted");
    check(queue_pop(&q, &v) == 0 && v == 7, "dequeue(&q, &v)", "-> 7");
}

/* ------------------------------------------------------------------ *
 *  무작위 스트레스 — 모델 큐(단순 배열)와 매 연산 대조
 * ------------------------------------------------------------------ */

static int stress(unsigned seed)
{
    const int CAP = 9;          /* 실제 용량은 8 */
    const int ROUND = 30000;

    Queue q;
    int   buf[9];
    int   model[8];             /* 모델: 앞에서 빼고 뒤에 넣는 단순 배열 */
    int   model_n = 0;
    int   i, j;

    srand(seed);
    if (set_queue(&q, buf, CAP) != 0) return 0;

    for (i = 0; i < ROUND; i++) {
        if (rand() % 2 == 0) {                          /* enqueue */
            int x  = rand() % 2000 - 1000;
            int rc = queue_push(&q, x);

            if (model_n == CAP - 1) {
                if (rc == 0) { printf("    i=%d: overflow 가 통과했다\n", i); return 0; }
            } else {
                if (rc != 0) { printf("    i=%d: enqueue 가 실패했다\n", i); return 0; }
                model[model_n++] = x;
            }
        } else {                                        /* dequeue */
            int v  = 0;
            int rc = queue_pop(&q, &v);

            if (model_n == 0) {
                if (rc == 0) { printf("    i=%d: underflow 가 통과했다\n", i); return 0; }
            } else {
                if (rc != 0) { printf("    i=%d: dequeue 가 실패했다\n", i); return 0; }
                if (v != model[0]) {
                    printf("    i=%d: FIFO 깨짐 want %d got %d\n", i, model[0], v);
                    return 0;
                }
                for (j = 1; j < model_n; j++) model[j-1] = model[j];
                model_n--;
            }
        }

        if (queue_empty(&q) != (model_n == 0)) {
            printf("    i=%d: empty 판정 불일치 (model_n=%d)\n", i, model_n);
            return 0;
        }
        /* head/tail 이 불변식을 지키는가 */
        if (q.head < 0 || q.head >= q.capacity || q.tail < 0 || q.tail >= q.capacity) {
            printf("    i=%d: 인덱스가 범위를 벗어남 h=%d t=%d\n", i, q.head, q.tail);
            return 0;
        }
    }
    return 1;
}

/* ------------------------------------------------------------------ */

int main(void)
{
    unsigned seed = (unsigned)time(NULL);

    printf("=== CLRS 10.1  queue (ring buffer, 한 칸 버리기) ===\n");
    printf("seed = %u\n", seed);

    test_init();
    test_textbook_10_1_3();
    test_capacity();
    test_wraparound();
    test_underflow();
    test_null();

    printf("\n[7] 무작위 스트레스 — 30000 연산, cap=9, 모델 큐와 대조\n");
    check(stress(seed), "stress", "매 연산이 모델과 일치");

    printf("\n=== %d / %d PASS ===\n", g_pass, g_pass + g_fail);
    return g_fail == 0 ? 0 : 1;
}
