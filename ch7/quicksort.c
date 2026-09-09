#include "quicksort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   


// 루프 불변식 : 배열이 매 루프마다 왼쪽과 오른쪽으로 구분이 되어있다. 왼쪽이 피벗보다 작은 값 오른쪽이 큰 값들로 이루어져 잇다. 
// 해당 인덱스 번호를 기준으로 위의 규칙이 적용된다. 

static int cur_depth = 0;
static int max_depth = 0;

void reset_depth(void)
{
    cur_depth = 0;
    max_depth = 0;
}

int get_max_depth(void)
{
    return max_depth;
}


/* ---- 계측 (알고리즘 아님) ------------------------------------------
 * partition 루프 안의 비교 횟수. 9.2 의 기대 Θ(n) 을 관측하기 위해 추가.
 * static 으로 숨기고 reset/get 만 노출한다 (reset_depth 패턴과 동일).
 * 전역을 그대로 열면 밖에서 실수로 대입할 수 있다.
 * ------------------------------------------------------------------ */
static long comparison_count = 0;

void reset_comparisons(void)
{
    comparison_count = 0;
}

long get_comparisons(void)
{
    return comparison_count;
}


static void swap(int *arr, int p, int r)
{
    int replace;

    replace = arr[p];
    arr[p] = arr[r];
    arr[r] = replace;
}


int partition(int arr[], int p, int r)
// pivot 이 최종적으로 자리 잡은 인덱스를 반환한다.
// (마지막 줄에서 swap 을 하고 나서 반환하므로, 그 자리에 있는 값은 pivot 이다.
//  "작은 값들의 마지막 인덱스"가 아니다 — 그건 반환값 - 1 이다. 9.2 에서 이 오해가
//  세 곳에 파급되는 버그를 유발했다.)
{
    int less_num_idx = p-1;
    int pivot = arr[r];

    for(int i =p; i < r; i++)
    {
        comparison_count++;   /* 계측: 아래 비교는 참이든 거짓이든 이미 일어났다.
                                 if 안에 두면 "분기 선택 횟수"를 센다 (COWORK §8 13번) */
        if(arr[i] <= pivot)
        {
            less_num_idx++;
            swap(arr, less_num_idx, i);
     }
    }
    swap(arr, less_num_idx+1, r);
    

    return less_num_idx+1;
}


int randomized_partition(int arr[], int p, int r)
{

    int random_num = rand();

    int random_idx = random_num%(r-p+1) + p;

    swap(arr, r, random_idx);

    return partition(arr, p, r);
}





void quicksort(int arr[], int p, int r)
//재귀식을 이용해서 연산을 지속한다. 
{
    cur_depth++;
    if (cur_depth > max_depth) max_depth = cur_depth;

    if(r <= p)
        {
            cur_depth--; 
            return;
        }

    int q;
    q = partition(arr, p, r);

    //재귀식의 종료 조건은 r 과 p 가 정확히 결정될 때 즉 하나만 남을 경우에
    // 반환한 q 값이 마지막에 위치한 인덱스일 경우에 
    

    quicksort(arr, p, q-1);
    quicksort(arr, q+1, r);

    cur_depth--; 
}

void randomized_quicksort(int arr[], int p, int r)
{
        if(r <= p)
        return ;

    int q;
    q = randomized_partition(arr, p, r);

    //재귀식의 종료 조건은 r 과 p 가 정확히 결정될 때 즉 하나만 남을 경우에
    // 반환한 q 값이 마지막에 위치한 인덱스일 경우에 
    

    randomized_quicksort(arr, p, q-1);
    randomized_quicksort(arr, q+1, r);
}


int hoare_partition(int arr[], int p, int r)
{
    //pivot을 초기 값으로 한다. 
    //왼쪽에서 시작돼서 오른쪽으로 진행하고, 피봇보다 크거나 같은 수가 나올 떄까지 진행한다. 
    //오른쪽에서 시작돼서 왼쪽으로 진행하고, 피봇보다 작거나 같은 수가 나올 떼까지 진행하다. 
    //양 쪽에서 오는 수가 같거나 왼쪽에서 오는 수가 더 커질 경우에 종료한다. 
    int pivot = arr[p];
    int i = p - 1;  ///만약 i 도입 없이 진행할 경우에 어떻게 되는지 질문할 것
    int j = r + 1;
    while (1) 
    {
        do { j--; } while (arr[j] > pivot);
        do { i++; } while (arr[i] < pivot);
        if (i < j) swap(arr, i, j);
        
        else
        {
            
            return j;
            }
    }
}





void hoare_quicksort(int arr[], int p, int r)
//partition으로 재귀식 처리
{
    if(p >= r)
        return;

    int q = hoare_partition(arr, p, r);

    hoare_quicksort(arr, p, q);
    hoare_quicksort(arr, q+1, r);

}


void quicksort_tail(int arr[], int p, int r)
{
     cur_depth++;
    if (cur_depth > max_depth) max_depth = cur_depth;

    while(p < r)
    {
        int q = randomized_partition(arr, p, r);
        quicksort_tail(arr, q+1, r);
        r = q-1;
    }

    cur_depth--;
    return ;
}

void quicksort_opt(int arr[], int p, int r)
{
    cur_depth++;
    if (cur_depth > max_depth) max_depth = cur_depth;


    while(p < r)
    {
        int q = randomized_partition(arr, p, r);
        if((q-p) <= (r-q))
        {
            quicksort_opt(arr, p, q-1);
            p = q+1;

        }

        else
        {
            quicksort_opt(arr, q+1, r);
            r = q-1;
        }
    }
    cur_depth--;
}








