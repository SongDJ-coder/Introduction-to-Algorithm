#include "quicksort.h"
#include <stdio.h>

// 루프 불변식 : 배열이 매 루프마다 왼쪽과 오른쪽으로 구분이 되어있다. 왼쪽이 피벗보다 작은 값 오른쪽이 큰 값들로 이루어져 잇다. 
// 해당 인덱스 번호를 기준으로 위의 규칙이 적용된다. 

static void swap(int *arr, int p, int r)
{
    int replace;

    replace = arr[p];
    arr[p] = arr[r];
    arr[r] = replace;
}


int partition(int arr[], int p, int r)
//피봇 기준으로 작은 값들의 마지막 인덱스 번호를 반환한다. 
{
    int less_num_idx = p-1;
    int pivot = arr[r];

    for(int i =p; i < r; i++)
    {
        if(arr[i] <= pivot)
        {
            less_num_idx++;
            swap(arr, less_num_idx, i);
        }
    }
    swap(arr, less_num_idx+1, r);

    return less_num_idx+1;
}


void quicksort(int arr[], int p, int r)
//재귀식을 이용해서 연산을 지속한다. 
{
    if(r <= p)
        return ;

    int q;
    q = partition(arr, p, r);

    //재귀식의 종료 조건은 r 과 p 가 정확히 결정될 때 즉 하나만 남을 경우에
    // 반환한 q 값이 마지막에 위치한 인덱스일 경우에 
    

    quicksort(arr, p, q-1);
    quicksort(arr, q+1, r);
}
















