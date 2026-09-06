
#include "randomized_select.h"
#include "../../ch7/quicksort.h"
#include <stdlib.h>
#include <stdio.h>


static void swap(int *arr, int p, int r)
{
    int replace;

    replace = arr[p];
    arr[p] = arr[r];
    arr[r] = replace;
}


static int less_num(int *arr,int n, int number)
{
    int num = 0;

    for(int i = 0; i < n; i++)
    {
        if(arr[i] <= number) num++;
    }

    return num;
}






static int randomized_helper(int * arr, int p, int r , int i)
//int * arr, size_t n, int i, int* result
{
    



    int num = randomized_partition(arr, p, r);


    int less_num = num - p + 1 ;


    if( less_num == i) return arr[num];



    else if(less_num < i)
    {
        return randomized_helper(arr, num+1, r, i-less_num);
    }

    else
    {
        return randomized_helper(arr, p, num-1, i);
    }



}

int randomized_select(int * arr, size_t n, int i, int* result)
{
    int p = 0;
    int r = (int)n -1;


    if(i <= 0 || (r - p + 1) < i)
        return 1;

    if(arr == NULL)
        return 1;

    if(result == NULL)
        return 1;    

    *result = randomized_helper(arr, p, r, i);
    return 0;

}




/*

int partition(int arr[], int p, int r)
//피봇 기준으로 작은 값들의 마지막 인덱스 번호를 반환한다. 
{
    int less_num_idx = p-1;
    int pivot = arr[r];

    // 파일 상단에
    static long partition_count = 0;
    // partition 안에서
    partition_count++;

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


int randomized_partition(int arr[], int p, int r)
{

    int random_num = rand();

    int random_idx = random_num%(r-p+1) + p;

    swap(arr, r, random_idx);

    return partition(arr, p, r);
}

*/