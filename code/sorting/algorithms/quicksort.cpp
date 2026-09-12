

#include "quicksort.h"



#include <bits/stdc++.h>
using namespace std;

/*
 * Quick Sort

 * Algorithm reference:
 * C. A. R. Hoare, "Quicksort", The Computer Journal, 1962.
 *
 * A divide-and-conquer sorting algorithm that selects a pivot
 * and partitions the array into regions around it. The partition are then
 * recursively sorted
 *
 * Time Complexity : O(nlogn)
 * Worst Case: O(n^2)

 * C++ implementation based on:
 * GeekforGeeksm "Quick Sort".
 * https://www.geeksforgeeks.org/cpp/cpp-program-for-quicksort/

*/

int partition(vector<int>& vec, int low, int high) {

    // Selecting the last element as the pivot
    int pivot = vec[high];

    // Index of the element smaller than the pivot
    int i = low - 1;

    for (int j = low; j < high; j++) {

        // If the current element is smaller than
        // or equal to the pivot
        if (vec[j] <= pivot) {
            i++;
            swap(vec[i], vec[j]);
        }
    }

    // Place the pivot in its correct position
    swap(vec[i + 1], vec[high]);

    // Return the partition index
    return i + 1;
}

void quickSort(vector<int>& vec, int low, int high) {

    // Base case
    if (low < high) {

        // Partition the array
        int pi = partition(vec, low, high);

        // Sort elements before the pivot
        quickSort(vec, low, pi - 1);

        // Sort elements after the pivot
        quickSort(vec, pi + 1, high);
    }
}

