#include "calculate.h"

float calculate_tour_cost(unsigned int tour_size, int **assymmetric_distances, int *tour) {
    float cost = 0.0;
    
    for (unsigned int i = 0; i < tour_size - 1; i++) {
        cost += assymmetric_distances[tour[i]][tour[i + 1]];
    }
    
    return cost;
}

float calculate_total_penalty(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, int *tour) {
    bool *visited = (bool *)allocate_vector(sizeof(bool), num_all_cities);
    float total_penalty = 0.0;
    
    for (unsigned int i = 0; i < tour_size; i++) {
        visited[tour[i]] = true;
    }
    
    for (unsigned int i = 0; i < num_all_cities; i++) {
        if (!visited[i]) {
            total_penalty += all_cities[i].penalty;
        }
    }
    
    free(visited);
    return total_penalty;
}

float calculate_objective_function(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, int **assymmetric_distances, int *tour) {
    float cost = calculate_tour_cost(tour_size, assymmetric_distances, tour);
    float total_penalty = calculate_total_penalty(all_cities, tour_size, num_all_cities, tour);

    return cost + total_penalty;
}