
#include <stdio.h>
#include <stdlib.h>


int main(void)
{



    int n;

    printf("input the number of your list");

    scanf("%d", &n);
    if(n < 0)
    {
        printf("양수를 입력하시오");
        return 1;
    }

    int *arr = malloc(sizeof(int)*n);
    //n 에 음수가 들어갈 경우에 메모리는 여전히 양수로 변환하기 때문에 엄청 큰 공간이 할당된다. 

    for(int i =0; i < n; i++)
    {
        scanf("%d", &arr[i]);
    }

    
    
    int key;
    int num;



    //i 번쨰 인덱스에 있는 값이 지속적으로 바뀐다. 이에 따라서 key 값을 정하고 시작하면 수월하게 비교할 수 있다. 
    for(int i = 1; i < n; i++)
    {
        key = arr[i];
        num = 1;
        while(key < arr[i-num])
        {
            arr[i-num+1] = arr[i-num];
            num ++;

        }
        //들어갈 인데스 위치를 볼 때는 조건식을 먼저 살펴 본댜. 그게 인덱스 위치를 정하기 유리하다

        arr[i-num + 1] = key; 

    }


    for(int i2 = 0; i2 < n; i2++)
    {
        printf("%d", arr[i2]);
        printf(" ");
    }

    free(arr);

    return 0;


}