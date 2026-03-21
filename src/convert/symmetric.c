#include "symmetric.h"

void convert_to_symmetric(problem *prob, solution *sol) {
    unsigned int n = sol->num_visited_cities;
    unsigned int new_size = 2 * n;
    unsigned int idx_i, idx_j;

    sol->symmetric_distances = allocate_matrix(new_size, new_size);

    for (unsigned int i = 0; i < new_size; i++) {
        for (unsigned int j = 0; j < new_size; j++) {
            if (i == j)
                sol->symmetric_distances[i][j] = 0;
            else
                sol->symmetric_distances[i][j] = 9999999;
        }
    }
    
    for (unsigned int i = 0; i < n; i++) {
        idx_i = sol->visited_cities[i].id;

        for (unsigned int j = 0; j < n; j++) {
            idx_j = sol->visited_cities[j].id;

            // i -> j_linha = i -> j
            sol->symmetric_distances[i][j + n] = prob->assymmetric_distances[idx_i][idx_j];
            
            // i_linha -> j = j -> i
            sol->symmetric_distances[i + n][j] = prob->assymmetric_distances[idx_j][idx_i];
        }
        
        sol->symmetric_distances[i][i + n] = 0;
        sol->symmetric_distances[i + n][i] = 0;
    }
}