/* CLRS 10.1 — stack 동작 확인용 main (Claude 작성)
 *
 * 교재 연습문제 10.1-1 의 연산 순서를 그대로 돌리면서
 * 매 단계의 배열 상태와 top 을 찍는다.
 */
#include <stdio.h>
#include "stack.h"

#define CAP 6

/* 배열 전체를 찍되, 스택에 실제로 들어 있는 구간만 [ ] 로 감싼다.
 * top 위쪽은 죽은 값이다 — 지우지 않고 남겨두는 걸 눈으로 보려는 것. */
static void dump(const Stack *s, const char *label)
{
    size_t i;

    printf("  %-12s top=%zu  ", label, s->top);
    for (i = 0; i < s->capacity; i++) {
        if (i < s->top) printf("[%3d]", s->arr[i]);
        else            printf(" %3d ", s->arr[i]);
    }
    if (s->top == 0) printf("   <- 비어 있음");
    if (s->top == s->capacity) printf("   <- 가득 참");
    printf("\n");
}

static void do_push(Stack *s, int x)
{
    char label[32];
    int  rc = stack_push(s, x);

    snprintf(label, sizeof label, "push(%d)", x);
    if (rc != 0) printf("  %-12s 실패 (overflow)\n", label);
    else         dump(s, label);
}

static void do_pop(Stack *s)
{
    char label[32];
    int  v  = 0;
    int  rc = stack_pop(s, &v);

    if (rc != 0) {
        printf("  %-12s 실패 (underflow)\n", "pop()");
        return;
    }
    snprintf(label, sizeof label, "pop() -> %d", v);
    dump(s, label);
}

int main(void)
{
    int   buf[CAP] = { 0 };     /* 호출자가 배열을 준비한다 */
    Stack s;

    set_stack(&s, buf, CAP);

    printf("=== CLRS 10.1  stack ===\n");
    printf("capacity = %d,  [  ] 안이 실제 스택 원소\n\n", CAP);

    printf("[1] 연습문제 10.1-1 의 연산 순서\n");
    dump(&s, "(시작)");
    do_push(&s, 4);
    do_push(&s, 1);
    do_push(&s, 3);
    do_pop (&s);
    do_push(&s, 8);
    do_pop (&s);

    printf("\n[2] LIFO 확인 — 남은 것을 전부 빼본다\n");
    while (!stack_empty(&s))
        do_pop(&s);
    printf("  넣은 순서 4, 1  ->  나온 순서 1, 4\n");

    printf("\n[3] underflow — 빈 스택에서 pop\n");
    do_pop(&s);

    printf("\n[4] overflow — capacity(%d) 를 넘겨서 push\n", CAP);
    {
        int i;
        for (i = 1; i <= CAP + 2; i++)
            do_push(&s, i * 10);
    }

    printf("\n[5] 가득 찬 상태에서도 pop 은 정상\n");
    do_pop(&s);
    do_push(&s, 99);

    return 0;
}
