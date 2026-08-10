#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void shuffle(int * arr, int n)
{
    //i를 i부터 n까지 수 중에서 하나를 뽑아서 i 번쨰 인덱스와 뽑힌 수의 인덱스를 바꿔준다. 
    // i는 첫 번째 인덱스 0부터 진행해서 n-1까지

    int swap_num;
    int tmp;

    for(int i= 0; i < n; i++)
    {
        swap_num = (rand()%(n-i) + i);

        tmp = arr[i];

        arr[i] = arr[swap_num];
        arr[swap_num] = tmp;

    }
}





int main(void)
{
    srand(time(NULL));

    //사용자한테 배열 받기
    int n;

    scanf("%d", &n);
    
    int * arr = malloc(sizeof(int)*n);

    for(int i = 0; i < n; i++)
    {
        scanf("%d", &arr[i]);
    }


    //받은 배열을 무작위로 셔플하기
    shuffle(arr, n);



    //배열을 출력하기
    for(int i =0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
}