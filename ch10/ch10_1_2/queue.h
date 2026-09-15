#ifndef QUEUE_H
#define QUEUE_H



typedef struct {
    int * arr;
    int capacity;
    int head;
    int tail;

} Queue;

int set_queue(Queue * queue, int *arr, int size);

int queue_empty(Queue * queue);

int queue_push(Queue * queue, int input);

int queue_pop(Queue * queue, int * result);


#endif