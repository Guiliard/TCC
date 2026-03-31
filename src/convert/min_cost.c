#include "min_cost.h"

void convert_tour_to_min_cost(problem *prob, solution *sol) {
    int *reverse_tour = (int *)allocate_vector(sizeof(int), sol->tour_size);
    
    for (int i = 0; i < sol->tour_size; i++) {
        reverse_tour[i] = sol->tour[sol->tour_size - 1 - i];
    }

    double cost_reverse = calculate_tour_cost(sol->tour_size, prob->asymmetric_distances, reverse_tour);

    if (cost_reverse < sol->tour_cost) {
        memcpy(sol->tour, reverse_tour, sol->tour_size * sizeof(int));
        sol->tour_cost = cost_reverse;
    }
    
    free(reverse_tour);
}