#include <stdlib.h>
#include "stack.h"



void set_stack(Stack * stack, int * arr, size_t n)
{
    stack->arr = arr;
    stack->capacity = n;
    stack->top = 0;

    return;
}

int stack_push(Stack * stack, int input)
{
    if(stack == NULL) return 1;
    //stack 이 다 차 있는지 먼저 확인
    if(stack->top >= stack->capacity) return 1;

    //stack 이 다 차 있지 않으면 top 하나 늘려서 넣어준다. 
    stack->top++;

    //배열 안에 숫자 넣기
    stack->arr[stack->top-1] = input;

    
    return 0;
}




int stack_pop(Stack * stack, int *result)
{
    if(stack == NULL || result == NULL) return 1;
    if(stack_empty(stack)) return 1;

    *result = stack->arr[stack->top-1];
    stack->top--;

    return 0;
}

int stack_empty(Stack * stack)
{
    if(stack == NULL) return 1;
    if(stack->top == 0) return 1;
    
    return 0;
}

