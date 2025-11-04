#include "symmetric.h"

void convert_to_symmetric(int ***distances, city *initial_solution, unsigned int num_initial_solution, int ***symmetric_distances) {
    unsigned int n = num_initial_solution;
    unsigned int new_size = 2 * n;
    unsigned int idx_i, idx_j;

    *symmetric_distances = allocate_matrix(new_size, new_size);

    for (unsigned int i = 0; i < new_size; i++) {
        for (unsigned int j = 0; j < new_size; j++) {
            if (i == j)
                (*symmetric_distances)[i][j] = 0;
            else
                (*symmetric_distances)[i][j] = 9999999;
        }
    }
    
    for (unsigned int i = 0; i < n; i++) {
        idx_i = initial_solution[i].index_city;

        for (unsigned int j = 0; j < n; j++) {
            idx_j = initial_solution[j].index_city;

            // i -> j_linha = i -> j
            (*symmetric_distances)[i][j + n] = (*distances)[idx_i][idx_j];
            
            // i_linha -> j = j -> i
            (*symmetric_distances)[i + n][j] = (*distances)[idx_j][idx_i];
        }
        
        (*symmetric_distances)[i][i + n] = 0;
        (*symmetric_distances)[i + n][i] = 0;
    }
}