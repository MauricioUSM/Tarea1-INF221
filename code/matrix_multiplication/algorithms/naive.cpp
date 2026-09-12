#include <vector>
#include <iostream>

#include "naive.h"


using namespace std;



/*
 * Naive Matrix Multiplication
 * 
 * Algorithm and complexity reference:
 * Stanford University, CS161 - Design and Analysis of Algorithms.
 * The naive solution is that we will multiply row by column to get each
 * element of the new matrix. Each new element of the new matrix is a sum of a row
 * multiplied by a column, which takes n time, and there are n
 * 2 new element to compute,
 * resulting in a runtime of O(n^3).

 * C++ implementation based on:
 * GeekforGeeksm "Quick Sort".
 * https://www.geeksforgeeks.org/dsa/strassens-matrix-multiplication/
 */



vector<vector<int>> naive(vector<vector<int>> &mat1, 
                                        vector<vector<int>> &mat2) {
    
    int n = mat1.size(), m = mat1[0].size(), 
                                    q = mat2[0].size();        

    // Initialize the result matrix with 
    // dimensions n×q, filled with 0s
    vector<vector<int>> res(n, vector<int>(q, 0));

    // Loop through each row of mat1
    for (int i = 0; i < n; i++) {
        
        // Loop through each column of mat2
        for (int j = 0; j < q; j++) {
            
            // Compute the dot product of 
            // row mat1[i] and column mat2[][j]
            for (int k = 0; k < m; k++) {
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    return res;
}
