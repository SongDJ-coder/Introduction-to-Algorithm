#include <stdio.h>
#include <stdlib.h>
#include "counting_sort.h"


int counting_sort(const int *arr, size_t n, int *result_array, int k)
{
    // k를 이용한 자연수 배열 C선언하기
    int *numbers = calloc(k+1, sizeof(int));

    if(numbers == NULL) return -1;


    // result_array = calloc(n, sizeof(int));

    // if(result_array == NULL) return -1
    //B는 호출자가 이미 할당해서 넘겨준 거라 함수가 다시 할당할 이유가 없다. 다시 할당하면 (a) 호출자의 배열은 안 채워지고 (b) 새로 잡은 메모리는 누수된다.
    //원래 배열에 있는 수들을 하나씩 돌면서 numbers 배열 갱신하기

    for(int i = 0; i < n; i++)
    {
        numbers[arr[i]]++;
    }

    // 갱신이 완료된 배열의 누적 합으로 변경한다.
    for(int i = 1; i < k+1; i ++)
    {
        numbers[i] = numbers[i-1]+numbers[i];
    }

    //배열을 뒤에서 읽어오면서 numbers 값을 인덱스 삼아서 result_array 에 담는다.
    for(int i = n-1; i >= 0; i--)
    {
        int idx = numbers[arr[i]] - 1;

        result_array[idx] = arr[i];
        numbers[arr[i]]--;
    }
    //for 문에서 i--의 순서가 어느 순간에 일어나느가?

    free(numbers);

    return 0;
}




/*

counting sort의 구현 : 원래 배열과 범위를 나타내느 k를 알고있다.

k 까지 배열을 선언한다. 0부터 k 까지 선언을 한다.

원래 배열을 하나씩 돌먄사 k 까지 적힌 배열의 인덱스 수를 늘린다

갱신된 k 까지의 배열을 가지고 누적합을 구한다.

배열을 뒤에서 읽어오면서 해당 인덱스에 적힌 수를 가지고 새롭게 할당된 영역의 인덱스에 넣는다.

*/
