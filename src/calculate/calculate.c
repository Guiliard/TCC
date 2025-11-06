#include "calculate.h"

float calculate_tour_cost(int *tour, unsigned int tour_size, int **distances) {
    float cost = 0.0;
    
    for (unsigned int i = 0; i < tour_size - 1; i++) {
        cost += distances[tour[i]][tour[i + 1]];
    }
    
    return cost;
}

float calculate_total_penalty(int *tour, unsigned int tour_size, city *cities, unsigned int num_cities) {
    bool *visited = (bool *)allocate_vector(sizeof(bool), num_cities);
    float total_penalty = 0.0;
    
    for (unsigned int i = 0; i < tour_size; i++) {
        visited[tour[i]] = true;
    }
    
    for (unsigned int i = 0; i < num_cities; i++) {
        if (!visited[i]) {
            total_penalty += cities[i].penalty;
        }
    }
    
    free(visited);
    return total_penalty;
}

float calculate_objective_function(int *tour, unsigned int tour_size, int **distances, city *cities, unsigned int num_cities) {
    float cost = calculate_tour_cost(tour, tour_size, distances);
    float total_penalty = calculate_total_penalty(tour, tour_size, cities, num_cities);

    return cost + total_penalty;
}