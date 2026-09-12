
#include "mergesort.h"

#include <iostream>
#include <vector>


/*
 * Merge sort implementation based on the pseudocode provided by:
 * Northern Illinois University, "Merge Sort Algorithm"
 * Data Structures and Algorithms in C++.
 * 
 * Algorithm based in divide and conquer.
 * The array is divided recursively in two halves and, once they 
 * sorted, both halves are combined through the merge function
 *
 * Time Complexity = O(nlogn)
 * Space complexity = O(n)
 * 
 *
 * Source;
 * https://faculty.cs.niu.edu/~mcmahon/CS241/Notes/Sorting_Algorithms/merge_sort.html

*/


void merge(std::vector<int>& A, int left, int mid, int right){
    
    std::vector<int> temp(right - left + 1);

    int i = left;
    int j = mid + 1;
    int k = 0;

    while(i <= mid && j <= right){
        if (A[i] < A[j]){
            temp[k] = A[i];
            i++;
        }else{
            temp[k] = A[j];
            j++;
        }
        k++;
    }
    while( i <= mid){
        temp[k] = A[i];
        i++;
        k++;
    }

    while( j <= right){
        temp[k] = A[j];
        j++;
        k++;
    }


    for(int x = 0 ; x < temp.size(); x++){
        A[left + x] = temp[x];
    }



}

void mergeSort(std::vector<int>& arr, int left, int right){

    if(left >= right){
        return;
    }

    int mid = left + (right - left) / 2;
    mergeSort(arr,left,mid);
    mergeSort(arr,mid+1,right);
    merge(arr,left,mid,right);
}


