#include "three_way.h"
#include "../../ch7/quicksort.h"   /* randomized_partition */
#include <stdlib.h>                /* NULL */
#include <limits.h>                /* INT_MAX */


/* ---- 파일 내부 유틸 --------------------------------------------------
 * ch7 의 swap 은 static 이라 이 파일에서 보이지 않는다. 직접 둔다. */
static void swap(int *arr, int a, int b)
{
    int t = arr[a];
    arr[a] = arr[b];
    arr[b] = t;
}

/* A[p..r] 을 오름차순 정렬. 기저 사례(원소 5개 이하)에서만 쓴다 */
static void insert_sort(int *arr, int p, int r)
{
    for (int j = p + 1; j <= r; j++)
    {
        int key = arr[j];
        int k = j - 1;
        while (k >= p && arr[k] > key)
        {
            arr[k + 1] = arr[k];
            k--;
        }
        arr[k + 1] = key;
    }
}

// 3 way 알고리즘 원리 

//partition으로 뽑는 값을 일정한 범주로 고정


//구체적인 방법 (5개씩 나눠서 각 범주에서 중앙 값을 하나 뽑는다.)

//뽑힌 값들을 대상으로 다시 중앙 값을 계산한다

//최종 중앙 값 계산으로 얻은 값을 순회를 거쳐서 나올 때까지 진행하고 나오면 가장 뒤로 보낸다. 

// 7 장에서 짠 partion 과 같은 원리를 이용한다. 

//단 같은 숫자가 있을 수 있다. 같은 숫자가 나올 때는 파티션에 편입 하면서 3way로 만든다. 

// partion의 범주가 i 에 속하면 거기서 끝낸다. 

//i 에 속하지 않으면 안으로 들어 가거나 바깥 쪽에 있는 범주로 계산을 이어간다. 






static int median_helper_iter(int * arr, int p, int r , int i)
{
    int start_point = p;
    int end_point = r;

    int target_point = i;

    int pivot;      /* 값이 아니라 인덱스다. ch7 partition 안의 pivot 은 값이니 혼동 주의 */

    while(1)
    {
        pivot = randomized_partition(arr, start_point, end_point);

        int less_num = pivot - start_point + 1;

        if(less_num == target_point) return pivot;



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


// 우선 5개씩 나눠서 배열 당 중앙 값을 배열의 앞으로 이동한다. 
static int gather_median(int* arr,int start_idx, int end_idx)
//모은 중앙값의 **개수**를 반환한다 (인덱스가 아니다).
//중앙값들은 A[start_idx .. start_idx + 반환값 - 1] 에 놓인다.
{
    if((end_idx - start_idx) < 4)
    {
        return 0;
    }

    int median_array_idx = 0;


    for(int i = 4 + start_idx; i <= end_idx; i += 5)
    {
        //한 바퀴 돌 때마다 앞에 인덱스로 위치를 이동해준다. 
        int res_median_idx = median_helper_iter(arr, i-4, i, 3); 
        swap(arr, start_idx + median_array_idx, res_median_idx);
        median_array_idx++;
    }

    return median_array_idx;
}


// i 번째에 해당하는 값을 구하는 가지 방법
//배열이 주어진다. 
//5개씩 묶어서 배열을 변경해 앞으로 보낸다. 

//중앙 값들이 모인 배열에서 재귀 함수를 이용해서 값을 구한다. 

//구한 중앙 값을 기준으로 3 way_partion을 진행한다. 

//i 가 해당 위치에 있는지 확인한다. 

//해당 범주에 있으면 반환한다. 


int three_way(int * arr, int p, int r, int i)
{
    if (r - p + 1 <= 5) { insert_sort(arr, p, r); return arr[p + i - 1]; }


    int m = gather_median(arr, p, r); //중앙값 m 개가 A[p .. p+m-1] 로 모인다

    /* m 개 중 아래쪽 중앙값의 1-based 순위는 (m+1)/2.  m=4 -> 2, m=5 -> 3 */
    int median  = three_way(arr, p, p+m-1, (m+1)/2);

    int less_num = 0;
    int same_num = 0;

    for (int i2 = p; i2 < r+1; i2++)
    {
        if(arr[i2] <  median )
        {
            int L = p + less_num;
            int G = p + less_num + same_num;

            swap(arr, i2, G); 
            swap(arr, G, L);

            less_num++;
        }
        else if(arr[i2] == median)
        {
            swap(arr, (less_num+p+same_num), i2);
            same_num++;
        }
    }
    //루프 불변식은 median 보다 작은 값은 왼쪽으로 큰 값은 으른쪽으로 정렬이 된 채로 진행이 된다.

    /*        p              p+less        p+less+same           r
     *        +--------------+-------------+--------------------+
     *        |    < x       |    == x     |       > x          |
     *        +--------------+-------------+--------------------+
     * 순위:    1 … less        less+1 …      less+same+1 …  n
     *                          less+same                        */

    if(i > less_num && i <= less_num+same_num)
    {
        return median;
    }

    else if(i <= less_num)
    {
        return three_way(arr, p, p+less_num-1, i);
    }

    else
    {
        /* 왼쪽에 남기고 가는 원소 수 = less_num + same_num */
        return three_way(arr, p+less_num+same_num, r, i-(less_num+same_num));
    }


}


/* ---- 공개 진입점 ----------------------------------------------------
 * i 는 1-based (i = 1 이 최솟값).
 * 반환 0 성공 / 0 이 아닌 값 실패.  값은 *result 로 나온다.
 * arr 을 재배치한다 (qsort 방식). 원본 보존은 호출자 책임. */
int three_way_select(int *arr, size_t n, int i, int *result)
{
    if (arr == NULL)            return 1;
    if (result == NULL)         return 1;
    if (n == 0)                 return 1;
    if (n > (size_t)INT_MAX)    return 1;   /* 헬퍼가 int p, int r 을 쓴다 */
    if (i <= 0)                 return 1;
    if ((size_t)i > n)          return 1;   /* i <= 0 을 먼저 걸러야 승격이 안전하다 */

    *result = three_way(arr, 0, (int)n - 1, i);
    return 0;
}








