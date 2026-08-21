#ifndef QUICK_SORT_H
#define QUICK_SORT_H


int partition(int arr[], int p, int r);
void quicksort(int arr[], int p, int r);


void randomized_quicksort(int arr[], int p, int r);

int randomized_partition(int arr[], int p, int r);

int hoare_partition(int arr[], int p, int r);

void hoare_quicksort(int arr[], int p, int r);


#endif