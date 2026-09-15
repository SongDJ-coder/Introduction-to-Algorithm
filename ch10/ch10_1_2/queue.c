#include "queue.h"
#include <stdlib.h>






int queue_empty(Queue * queue)
{

    if(queue == NULL) return 1;

    if(queue->head == queue->tail) return 1;

    return 0;
}


int queue_push(Queue * queue, int input)
{
    if(queue == NULL) return 1;

    //배열이 꽉 찰 떄까지 진행할 수 있음. 
    //배열이 다 차 있음은 tail 이 head 꽁지까지 쫒아가 있으면 된다.
    //capacity 로 나눈 나머지가 순환의 기준이 되므로 나눈 나머지가 같은지 아닌지 비교한다.  
    if((queue->tail+1)%(queue->capacity) == queue->head) return 1;
    
    queue->arr[queue->tail] = input;
    //tail의 위치를 하나 추가한다.(capacity가 기준이 되므로 기준으로 나누는 형식으로 작동한다.)
    queue->tail = (queue->tail+1)%(queue->capacity);
    return 0;
}

int queue_pop(Queue * queue, int * result)
{
    if(queue == NULL || result == NULL) return 1;
    if(queue_empty(queue)) return 1;

    *result = queue->arr[queue -> head];
    queue-> head = (queue->head+1)%(queue->capacity);

    return 0;
}



int set_queue(Queue * queue, int *arr, int size)
{
    if(queue==NULL || arr==NULL) return 1;

    if(size <= 0) return 1;

    queue->arr = arr;
    queue->capacity = size;
    queue->head = 0;
    queue->tail = 0;

    return 0;
}

