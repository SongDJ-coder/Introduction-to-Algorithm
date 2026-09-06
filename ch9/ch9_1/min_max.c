#include "min_max.h"



size_t g_comparisons = 0;

int min_max(const int *arr, size_t n, int *min, int *max)   // 0 성공 / 1 실패
{

    //n 이 홀수일 경우, 짝수인 경우 

    //홀수일 경우 처음 값을 min, maxㄹ로 초기화 함. 
    
    //이후 둘 식 짝 지어서 최대 최소 후보를 구하고 그 값을 이전 min, max 후보랑 비교

    if(n == 0) return 1;
    size_t progress = 0;

    int min_num;
    int max_num;

    if(n%2 == 0)
    {
        g_comparisons++;                       /* 조건식이 평가되는 시점에 센다 */
        if(arr[0] < arr[1]) {min_num = arr[0]; max_num = arr[1];}
        else {min_num = arr[1]; max_num = arr[0];}

        progress += 2;
    }

    else
    {
        min_num = arr[0];
        max_num = arr[0];
        progress++;
    }

    
    int s;
    int l;


    while(progress+1 < n)
    {
        g_comparisons++;                       /* 짝 비교 */
        if(arr[progress] < arr[progress+1])
        {
            s = arr[progress];
            l = arr[progress+1];
        }
        else
        {
            l = arr[progress];
            s = arr[progress+1];
        }

        g_comparisons++;                       /* s vs min — 대입 여부와 무관하게 실행된다 */
        if( s < min_num)
            {min_num = s;
            }

        g_comparisons++;                       /* l vs max */
        if(l > max_num)
            {max_num = l;
            }

        progress += 2;
    }

    *min = min_num;
    *max = max_num;

    
    return 0;
}