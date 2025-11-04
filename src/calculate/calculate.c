#include "calculate.h"

double calculate_tour_cost(int *tour, unsigned int tour_size, int **distances) {
    if (tour_size == 0) return 0.0;
    
    double cost = 0.0;
    
    for (unsigned int i = 0; i < tour_size - 1; i++) {
        cost += distances[tour[i]][tour[i + 1]];
    }
    cost += distances[tour[tour_size - 1]][tour[0]];
    
    return cost;
}

double calculate_total_penalty(int *tour, unsigned int tour_size, city *cities, unsigned int num_cities) {
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

double calculate_objective_function(int *tour, unsigned int tour_size, int **distances, city *cities, unsigned int num_cities) {
    if (tour_size == 0) return 0.0;
    
    int *reverse_tour = (int *)allocate_vector(sizeof(int), tour_size);
    for (unsigned int i = 0; i < tour_size; i++) {
        reverse_tour[i] = tour[tour_size - 1 - i];
    }

    double cost_normal = calculate_tour_cost(tour, tour_size, distances);
    double cost_reverse = calculate_tour_cost(reverse_tour, tour_size, distances);
    double min_cost = min(cost_normal, cost_reverse);

    printf("Custo na direção normal: %.2f\n", cost_normal);
    printf("Custo na direção reversa: %.2f\n", cost_reverse);
    printf("Menor custo do tour: %.2f\n", min_cost);
    
    double total_penalty = calculate_total_penalty(tour, tour_size, cities, num_cities);
    
    free(reverse_tour);

    return min_cost + total_penalty;
}