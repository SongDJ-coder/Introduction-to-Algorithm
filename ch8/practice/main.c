#include <stdio.h>
#include <stdlib.h>

int compare_int(const void *a, const void *b)
{
    const int *pa = (const int *)a;
    const int *pb = (const int *)b;

    if (*pa < *pb)
        return -1;
    else if (*pa > *pb)
        return 1;
    else
        return 0;
}


int main(void)
{


int arr[8] = {2,8,7,1,3,5,6,4};
qsort(arr, 8, sizeof(int), compare_int);

for(int i =0; i < 8; i++)
{
    printf("%d", arr[i]);
}

}


