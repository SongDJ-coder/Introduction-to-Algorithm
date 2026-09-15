#ifndef STACK_H
#define STACK_H
#include <stddef.h>

typedef struct{
    int * arr;  //배열을 담고 있는 stack 구조
    size_t capacity; //배열의 크기 
    size_t top;  //제일 위에 있는 인덱스 + 1
} Stack;

int stack_push(Stack * stack, int input);

int stack_pop(Stack * stack, int *result);

int stack_empty(Stack * stack);

void set_stack(Stack * stack, int * arr, size_t n);



#endif   