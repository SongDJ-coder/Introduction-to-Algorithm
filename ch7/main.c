#include "quicksort.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void PrintArray(int * arr, int n)
{
    for(int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}



int main(void)
{
    srand(time(NULL));
    
    int arr[10] = {2, 8, 7, 1, 3, 5, 6, 4};
    int arr2[10] = {1, 2, 3, 4, 5};
    int arr3[10] = {5, 4, 3, 2, 1};
    int arr4[10] = {3, 3, 3, 3, 3};
    int arr5[1] = {7};

    randomized_quicksort(arr, 0, 7);
    randomized_quicksort(arr2, 0, 4);
    randomized_quicksort(arr3, 0, 4);
    randomized_quicksort(arr4, 0, 4);
    randomized_quicksort(arr5, 0, 0);

    PrintArray(arr, 8);
    PrintArray(arr2, 5);
    PrintArray(arr3, 5);
    PrintArray(arr4, 5);
    PrintArray(arr5, 1);

    return 0;
}