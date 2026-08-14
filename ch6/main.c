#include <stdio.h>
#include <stdlib.h>
#include "HeapSort.h"


// /* 사용자에게 배열을 입력바는 함수*/
// static int* get_arr(int* size_out)
// {
//     printf("input your array number");
//     scanf("%d", size_out);

//     int * arr = malloc(sizeof(int)*(*size_out));

//     if (arr == NULL) {
//         return NULL;
//     }

//     printf("pinpout the array element");

//     for(int i = 0; i < *size_out; i++)
//     {
//         scanf("%d", &arr[i]);
//     }



//     printf("Your input is done");

//     return arr;
// }

static void print_arr(int *arr, int n, const char *label)
{
    printf("%s: ", label);
    
    for(int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");
}




int main(void)
{
    int lenght = 15;
    int arr[15] = {0};

    printf("Input number btween 0 - 15");
    int heap_size;

    scanf("%d", &heap_size);

    for(int i = 0; i < heap_size; i++)
    {
        scanf("%d", &arr[i]);
    }

    //사용자한테 배열 받기

    print_arr(arr, heap_size, "초기 배열");


    build_max_heap(arr, heap_size);
    print_arr(arr, heap_size, "max_heap으로 변결 후");


    heap_increase_key(arr, 6, 10000);
    print_arr(arr, heap_size, "6번 인덱스 값 수정 후");


    heap_insert(arr, &heap_size, lenght, 20);
    print_arr(arr, heap_size, "heap에 삽입 후");



}