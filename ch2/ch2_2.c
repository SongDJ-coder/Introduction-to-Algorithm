#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void Merge(int*arr, int* arr_replace, int start, int end)
{
    int left_idx = start;
    int right_idx = (start+end)/2 + 1;
    int k = start;
    int * for_replace;

    while(right_idx <= end && left_idx <= (start+end)/2)
    {
        if(arr[left_idx] > arr[right_idx])
        {
            arr_replace[k] = arr[right_idx];
            right_idx++;
        }
        else
        {
            arr_replace[k] = arr[left_idx];
            left_idx++;
        }
        k++;
    }

    
        while(left_idx <= (start+end)/2)
        {
            arr_replace[k] = arr[left_idx];
            left_idx++;
            k++;
        }

        while (right_idx <= end)
        {
            arr_replace[k] = arr[right_idx];
            right_idx++;
            k++;
        }        

        //이미 while 문에서 빈복문을 돌고 있기 떄문에 굳이 if else 문으로 감쌀 필요가 없음. 


    // for_replace = arr;
    // arr = arr_replace;
    // arr_replace = for_replace;
    // 선언한 변수도 결국 함수 내에서만 작동하는 지역 변수임. 즉 원하는 방식대로 변수의 주소 값으로 할당이 안될 것임. (많이 하는 실수이니 기억할 것)
    for (int i = start; i <= end; i++) 
    {
        arr[i] = arr_replace[i];
    }


}


void DividedSort(int* arr, int* arr_replace,int start,int end)
{
    //탈출 조건은 나눠진 배열이 길이가 1이거나 2일 때임. 
    if((end - start)<1)
        return;

    int mid = (start + end)/2;
    DividedSort(arr, arr_replace, start, mid);
    DividedSort(arr, arr_replace, mid+1, end);

    Merge(arr, arr_replace, start, end);
}

int main(void)
{
    //사용자한테 우선 배열을 할당 받기
    int arr_length;

    printf("input your list length");
    scanf("%d", &arr_length);

    int * arr = malloc(sizeof(int)*arr_length);

    for(int i = 0;i < arr_length; i++)
    {
        scanf("%d", &arr[i]);
    }
    //사용자한테 배열을 입력 받는 부분까지 완료

    // 병합 정렬을 하기 위한 여분의 배열 공간을 만들어서 초기화 해둠. 
    int* arr_replace = calloc(arr_length, sizeof(int));



    //배열이 2 또는 1이 될때까지 나눠야 함. 
    
    //나누기 위해서는 mid 값을 알아야 함. 
    int start = 0;
    int end = arr_length-1;

    

    DividedSort(arr, arr_replace, start, end);




    for(int i2=0; i2 < arr_length; i2++)
    {
        printf("%d ", arr[i2]);
    }

    free(arr);
    free(arr_replace);
}
// 지금 이 코드의 경우애는 메모리 할당 연산 횟수를 줄이기 위해서 int max 를 사용하지 않음. 
//그러나 간단히 끝내기 위해서 메모리 여분 영역을 만들어서 그 자리에 int max 를 이용해서 절대 선택 되지 않게 설계할 수 있음 
