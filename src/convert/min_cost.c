#include "min_cost.h"

void convert_tour_to_min_cost(problem *prob, solution *sol) {
    int *reverse_tour = (int *)allocate_vector(sizeof(int), sol->num_visited_cities + 1);
    for (unsigned int i = 0; i < sol->num_visited_cities + 1; i++) {
        reverse_tour[i] = sol->tour[sol->num_visited_cities - i];
    }

    float cost_normal = calculate_tour_cost(sol->num_visited_cities + 1, prob->assymmetric_distances, sol->tour);
    float cost_reverse = calculate_tour_cost(sol->num_visited_cities + 1, prob->assymmetric_distances, reverse_tour);

    if (cost_reverse < cost_normal) {
        memcpy(sol->tour, reverse_tour, (sol->num_visited_cities + 1) * sizeof(int));
    } 
    
    free(reverse_tour);
}