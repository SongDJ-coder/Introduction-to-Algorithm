#include "bucketsort.h"
#include <stdio.h>
#include <stdlib.h>





int bucket_sort(double * arr,double * result_arr, int n)
/*양의 실수 배열이 주어질 경우 해당 배열을 정해진 범위 내의 영역 안에 할당하여 배열하는 방법*/
{

    if (n==0) return 0;
    /*
    기본적인 원리
    
    사용자가 배열을 보내준다. 

    배열에 있는 수들의 최대 최소 값을 구한다. 

     최대 최소 값을숫자만큼 나눈다. 나눠 구한 값에 해당하는 구간의 길이를 하나 더 더해서 균등 분포가 가능한 칸 간격을 각 칸에 할당한다.
 
     해당 구간을 정수 형으로 변형하는 비율 값을 구한다. 
 
     배열 내 원소의 개수에 해당하는 배열을 생성한다.
 
     배열에 있는 각 수들을 뒤에서부터 끌고 와서 해당 비율들을 곱하고 소수 점을 제외했을 떄 나오는 인덱스 값을 증가한다. 
 
    숫자 값이 들어가 잇는 배열의 누적 합을 구한다. 
 
    TODO 뒤에서부터 하나씩 누적합 숫자 배열의 값을 하나씩 감소 하면서 합에 해당하는 인덱스 
     
    TODO 해당 범위에 있는 값들이 최초로 할당 받는 값
 
    TODO 2이상의 숫자가 누적합으로 할당된 배열 내에 있는 원소를 insertionsort 로 배열한다. 
    */

    //배열에 있는 수 중에 최대 최소 값을 구한다. 
    //예시 배열을 [0.1, 0.5, 0.2, 0.9, 0.46] 이라고 하자
    double max = arr[0];
    double min = arr[0];
    for (int i = 1; i < n; i++)
    {
        if(arr[i] <= min) min = arr[i];
        if(arr[i] > max) max = arr[i];
    }

    if(min == max) {
        for(int i =0; i <n; i++)
        {
            result_arr[i] = arr[i];
        }
        return 0;
    }
    //[0.1, 0.5, 0.2, 0.9, 0.46, 0.6] min : 0.1 max : 0.9

    //각 배열의 범위를 이용해서 인덱스에 할당하는 비율을 구하기   0.16
    // 최댓값 - 최솟값 = 각 숫자들의 간격  
    // 단 최소 값의 인덱스를 0과 1 사이로 할당하는 비율 상수를 통해서 최소 값이 0번 인덱스 값으로 기준을 잡게 되면 n 보다 하나 큰 n-1 간격으로 할당해야 한다. 


    double range_array = (max - min)/(n-1);



    //위에서 구한 간격을 간격 1로 만든다  1 / 0.16 = 6.25
    double multiple = 1 / range_array;

    //0으로 기준을 잡기 위해서 최솟값 기준치를 구해서 나중에 연산 시에 빼준다. [0.0     2.5     0.625   5.0     2.25    3.125]
    double zero_creteria = min * multiple;  

    //배열 내에 원소 개수에 할당하는 배열을 생성한다. (나중에 인덱스 값이 될 것이므로 calloc으로 할당한다.) 또한 ㅈ어수형으로 할당하낟. 
    
    int * idx_reference_arr = calloc(n, sizeof(int));


    //원 배열을 순회 하면서 인덱스 연산을 통해서 idx_reference_arr 의 각 인덱스 값을 하나씩 갱신한다. 

    for(int i = 0; i < n; i++)
    {
        //각 숫자에 비율 상수를 곱해서 인덱스로 할당해 해당 인덱스에 있는 값을 하나씩 더해서 갱신한다. 
        // reference에 들어가기 전에 int 형으로 치환한 배열[0       2       0       5       2       3 ]
        //[2, 0, 2, 1, 0, 1]
        idx_reference_arr[(int)(arr[i] *multiple - zero_creteria)]++;

    }
    


    //사용자에게 반환할 결과 값이 담긴 배열 구조를 만들기 
    //초기 값을 모두 0보다 작은 수로 할당하기 
    
    for(int i =0; i < n; i++)  result_arr[i] = -1;



    //갱신이 끝난 배열의 누적 합을 구한다. 
    for (int i = 1; i < n; i++)
    {
        idx_reference_arr[i] = idx_reference_arr[i-1] + idx_reference_arr[i];
    }
    //[2, 2, 4, 5, 5, 6]

    //누적 합이 종료된 이후 뒤에서부터 읽어온다. 그러나 이거는 겹치는 값에 대해서 inserting sort를 하기 때문에 굳이 뒤에서부터 가지고 올 필요가 없다. 
    for(int i = 0; i < n; i++)
    {
        //별도의 공간 할당 없이 구현하기
        //만약 0번 인덱스로 할당이 된 두 숫자가 서로 다른 순서에 저장이 되었다고 하자

        //처음 0번 인덱스가 나와서 해당 위치에 할당이 된다. 
        //0번 인덱스에 있는 수인 2번 인덱스 resulting_arr위치로 들어간다. 
        //result_arr에 있는  수는 아직 감소시키지 않다. 
        //해당 인덱스에 있는 값이 0보다 작을 경우에 바로 대입한다. 
        //해당 인덱스에 음수보다 큰 값이 존재할 경우에 비교한다. 
        //비교 후에 더 작은 수를 한 인덱스 낮춰서 넣는다. (음수인 값이 나올 떄까지 반복한다.)

        int idx_for_ref = (int)(arr[i] *multiple - zero_creteria);

        if(result_arr[idx_reference_arr[idx_for_ref] - 1] < 0) result_arr[idx_reference_arr[idx_for_ref] - 1] = arr[i]; //해당 인덱스에 있는 값이 0보다 작을 경우

        else
        {
            //위에서 이미 해당 인덱스에서 음수 값이 들어가 있지 않다는 게 밝혀짐. 

            double tmp = arr[i];
            double dum;
            int idx = idx_reference_arr[idx_for_ref] - 1;

            while(result_arr[idx-1] >=0)
            {
                if(result_arr[idx] > tmp) idx--;

                else
                {
                    dum = result_arr[idx];
                    result_arr[idx] = tmp;
                    tmp = dum;
                    idx--;
                }

            }

            if(result_arr[idx] < tmp)
                {
                    dum = result_arr[idx];
                    result_arr[idx] = tmp;
                    result_arr[idx-1] = dum;
                    idx--;
                }

            else
            {
                result_arr[idx-1] = tmp;
                idx--;
            }
            
                
            }

        }

    free(idx_reference_arr);

    return 0;
}