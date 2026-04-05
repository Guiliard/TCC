#include "min_cost.h"

void convert_tour_to_min_cost(problem *prob, solution *sol) {
    int *reverse_tour = (int *)allocate_vector(sizeof(int), sol->tour_size);
    int* positions_in_tour = allocate_vector(sizeof(int), prob->num_all_cities);
    
    for (int i = 0; i < sol->tour_size; i++) {
        reverse_tour[i] = sol->tour[sol->tour_size - 1 - i];
    }

    double cost_reverse = calculate_tour_cost(sol->tour_size, prob->asymmetric_distances, reverse_tour);

    if (cost_reverse < sol->tour_cost) {
        memcpy(sol->tour, reverse_tour, sol->tour_size * sizeof(int));
        sol->tour_cost = cost_reverse;
    }

    for(int i = 0; i < prob->num_all_cities; i++) {
        positions_in_tour[i] = -1;
    }

    for(int i = 0; i < sol->tour_size; i++) {
        int city_id = sol->tour[i];
        positions_in_tour[city_id] = i;
    }

    sol->city_pos_in_tour = positions_in_tour;
    
    free(reverse_tour);
}