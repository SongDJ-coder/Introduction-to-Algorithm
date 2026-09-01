#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "radix_sort.h"
#include "counting_sort.h"


int radix_sort(const int *arr, size_t n, int *result_array)
{
    if(n == 0) return 0;


    // 자릿수를 구해서 반복문으로 구현한다.
    //배열에서 최댓값을 찾기
    int max = arr[0];


    for(int i =1; i < n; i++)
    {
        if(max < arr[i])
            max = arr[i];
    }





    //최댓값의 자릿수 구하기
    int k = 1;
    int num = 0;

    while(max/k != 0)
    {
        k *= 10;
        num++;
    }
    if(num == 0) num++;

    //num 이 자릿수 역할을 함.

    //자릿수가 짝수이면 처음 배열 갱신 값을 result_array로 한다.
    //자릿수가 홀수이면 처음 배열 갱신 값을 arr로 한다.
    int div = 1;

    int *buf = calloc(n, sizeof(int));

    if (buf == NULL)
        {
            
            return -1;
        }



    int * dst;
    int * tmp;
    int * src;

    if(num % 2 == 0)  dst = buf;
    else dst = result_array;

    if (counting_sort_v2(arr, n, dst, 9, div) != 0) 
    {
        free(buf);
        return -1;
    }
    div *= 10;


    for(int i = 1; i < num; i++)                                                

    {
        tmp = (dst == result_array) ? buf : result_array;
        src = dst;
        dst = tmp;

    if (counting_sort_v2(src, n, dst, 9, div) != 0) 
        {
            free(buf);
            return -1;
        }

        div *= 10;


    }
    
    free(buf);

    return 0;




}

int radix_sort_char(const char**arr, size_t n, const char**result_array)
{
      if(n == 0) return 0;


    // 알파벳 수가 곧 반복 수임. 
    int num = strlen(arr[0]);

    

    //자릿수가 짝수이면 처음 배열 갱신 값을 result_array로 한다.
    //자릿수가 홀수이면 처음 배열 갱신 값을 arr로 한다.

    const char ** buf = malloc(sizeof(char*)* n);

    if (buf == NULL)
        {
            return -1;
        }


    //그러면 여기에도 const를 해줘야 되는건가? const char** 이렇게?
    const char ** dst;
    const char ** tmp;
    const char ** src;

    if(num % 2 == 0)  dst = buf;
    else dst = result_array;

    int pos = num -1;

    if (counting_sort_char(arr, n, dst, 25, pos) != 0) 
    {
        free(buf);
        return -1;
    }

    pos--;
   

    for(; pos >= 0; pos--)                                                

    {
        tmp = (dst == result_array) ? buf : result_array;
        src = dst;
        dst = tmp;

    if (counting_sort_char(src, n, dst, 25, pos) != 0) 
        {
            free(buf);
            return -1;
        }
    }
    
    free(buf);

    return 0;


}




int radix_sort_r(const int *arr, size_t n, int *result_array, int radix)
{
    if(n == 0) return 0;


    // 자릿수를 구해서 반복문으로 구현한다.
    //배열에서 최댓값을 찾기
    int max = arr[0];


    for(int i =1; i < n; i++)
    {
        if(max < arr[i])
            max = arr[i];
    }





    //최댓값의 자릿수 구하기
    long long int k = 1;
    int num = 0;

    while(max/k != 0)
    {
        k *= radix;
        num++;
    }
    if(num == 0) num++;

    //num 이 자릿수 역할을 함.

    //자릿수가 짝수이면 처음 배열 갱신 값을 result_array로 한다.
    //자릿수가 홀수이면 처음 배열 갱신 값을 arr로 한다.
    long long int div = 1;

    int *buf = calloc(n, sizeof(int));

    if (buf == NULL)
        {
            
            return -1;
        }



    int * dst;
    int * tmp;
    int * src;

    if(num % 2 == 0)  dst = buf;
    else dst = result_array;

    if (counting_sort_v3(arr, n, dst, div, radix) != 0) 
    {
        free(buf);
        return -1;
    }
    div *= radix;


    for(int i = 1; i < num; i++)                                                

    {
        tmp = (dst == result_array) ? buf : result_array;
        src = dst;
        dst = tmp;

    if (counting_sort_v3(src, n, dst, div, radix) != 0) 
        {
            free(buf);
            return -1;
        }

        div *= radix;


    }
    
    free(buf);

    return 0;




}