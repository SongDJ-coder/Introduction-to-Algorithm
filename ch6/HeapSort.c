# include "HeapSort.h"
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>

static void swap(int *arr, int idx_chage, int idx_chaged)
{
    int replace;

    replace = arr[idx_chage];

    arr[idx_chage] = arr[idx_chaged];

    arr[idx_chaged] = replace;
}

int get_left(int * arr, int heap_size, int idx)
{
    if((idx+1)*2 -1 >= heap_size)
        return -1;

    else 
        return (idx+1)*2 -1;
    
}

int get_right(int * arr, int heap_size, int idx)
{
    if((idx+1)*2  >= heap_size)
        return -1;

    else 
        return (idx+1)*2;
    
}


void max_heapify(int* arr, int heap_size, int idx)
{
    if(get_left(arr, heap_size, idx) < 0)
        return;
    //왼쪽 자식이 존재하지 않을 경우에 자동으로 오른쪽 자식도 존재하지 않음. 

    int left = get_left(arr, heap_size, idx);
    int right;
    if (get_right(arr, heap_size, idx) == -1)
    {
        right = left;
    }  
    else{
        right = get_right(arr, heap_size, idx);
    }
    // 오른쪽 자식과 왼쪽 자식을 설정하기 오른쪽의 경우에 음수 값이므로 양수 배열에서 고려하는 지금 상황에서는 무조건 작은 값으로 선택이 됨. 즉 고려할 필요가 없음. 

    if(arr[left] > arr[right])
    {
        if (arr[idx] < arr[left])
        {
            swap(arr, left, idx);

            max_heapify(arr, heap_size, left);
        }
    }
    else
    {
        if(arr[idx] < arr[right])
        {
            swap(arr, right, idx);
            max_heapify(arr, heap_size, right);
        }
    }


}

void build_max_heap(int *arr, int heap_size)
{
    int start_point = (heap_size-2)/2;

    for(int i = start_point; i >= 0; i--)
    {
        max_heapify(arr, heap_size, i);
    }
}

void heapsort(int *arr, int heap_size)
{
    build_max_heap(arr, heap_size);

        
    for(int i =heap_size-1; i >=1; i--)
    {
        swap(arr, 0, i);
        max_heapify(arr, i, 0);
        
    }
}




int  heap_maximum(int *arr, int heap_size)
// heap 변경 없이 최대 값을 반환하는 함수
{
    if(heap_size < 1)
    {
        printf("Your array doesn't have any elements");
        return -1;
    }
    return arr[0];
}



int  heap_extract_max(int *arr, int *heap_size)
//최대 값을 추출하는 함수
{
    if(*heap_size < 1)
        {
        printf("Your array doesn't have any elements");
        return -1;
        }
    else
    {
        int max;

        max = arr[0];

        swap(arr, 0, *heap_size-1);
        (*heap_size)--;
        max_heapify(arr, *heap_size, 0);

        return max;
    }
}










void heap_increase_key(int *arr, int idx, int key)
// 증가하는 상황만 고려
// 부모 인덱스 값이 더 클 경우에 끝
// 0번 인덱스 값이 되었을 때 끝
{
    int parent = (idx-1)/2;

    if(key < arr[idx])
        {
            printf("Error");
            return;
        }

    if((idx == 0) || (arr[parent] > key))
    {
        arr[idx] = key;
        return;
    }
   

    arr[idx] = arr[parent];

    heap_increase_key(arr, parent, key);
    
}









void heap_insert(int *arr, int *heap_size, int length, int key)
//값을 추가하는 함수
{
    if(*heap_size == length)
        return;

    arr[*heap_size] = INT_MIN;
    (*heap_size) ++;
    heap_increase_key(arr, *heap_size-1, key);
}