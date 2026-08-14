
#ifndef HEAPSORT_H
#define HEAPSORT_H

typedef int Data;


/* 오직 하나의 노드만 규칙을 어기고 있을 겅우에 딱 하나의 노드를 정상화 하는 로직 */
void max_heapify(int * arr, int heap_size, int idx);

int get_left(int * arr, int heap_size, int idx);

int get_right(int * arr, int heap_size, int idx);

void build_max_heap(int *arr, int n);

void heapsort(int *arr, int heap_size);

int  heap_maximum(int *arr, int heap_size);

int  heap_extract_max(int *arr, int *heap_size);

void heap_increase_key(int *arr, int idx, int key);

void heap_insert(int *arr, int *heap_size, int length, int key);

#endif
