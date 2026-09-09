
#include "randomized_select.h"
#include "../../ch7/quicksort.h"
#include <stdlib.h>     /* NULL */


/* 재귀 버전과 반복 버전을 둘 다 남긴다 (ch7 의 quicksort / quicksort_tail 과 같은 이유).
 * 둘은 완전히 같은 계약을 갖는다:
 *   A[p..r] 에서 i 번째로 작은 값을 반환. i 는 1-based.
 *   호출자는 1 <= i <= r-p+1 을 보장해야 한다 (진입점이 검증한다).
 *   배열을 재배치한다. */



/* --- 재귀 버전 --------------------------------------------------------
 * 두 재귀 호출 모두 return 직후에 할 일이 없다 = 꼬리 재귀.
 * 그래서 아래 반복 버전으로 기계적 변환이 가능하다. */
static int randomized_helper(int * arr, int p, int r , int i)
{
    int num = randomized_partition(arr, p, r);   // num 은 pivot 이 확정된 인덱스

    int less_num = num - p + 1 ;                 // pivot 의 순위 (1-based)

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


/* --- 반복 버전 (연습문제 9.2-3) ---------------------------------------
 * 재귀 깊이는 기대 O(lg n) 이지만 최악은 Theta(n) 이다.
 * n = 10^6 이면 프레임 100만 개 -> stack overflow.
 * 반복 버전은 프레임을 하나만 쓴다. */
static int randomized_helper_iter(int * arr, int p, int r , int i)
{
    int start_point = p;
    int end_point = r;

    int target_point = i;

    int pivot;      /* 값이 아니라 인덱스다. ch7 partition 안의 pivot 은 값이니 혼동 주의 */

    while(1)
    {
        pivot = randomized_partition(arr, start_point, end_point);

        int less_num = pivot - start_point + 1;

        if(less_num == target_point) return arr[pivot];



        else if(less_num < target_point)  
        {
            start_point = pivot+1;
            target_point = target_point - less_num;
        }
        

        else
        {
            end_point = pivot-1;
        }

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

    /* 두 버전은 같은 계약을 갖는다. 검증할 쪽을 골라 끼운다.
     *   *result = randomized_helper(arr, p, r, i);        // 재귀 — 검증 완료 14172/14172 */
    *result = randomized_helper_iter(arr, p, r, i);     /* 반복 (9.2-3) — 검증 중 */
    return 0;

}