#include "calculate.h"

float calculate_tour_cost(unsigned int tour_size, int **assymmetric_distances, int *tour) {
    float cost = 0.0;
    
    for (unsigned int i = 0; i < tour_size - 1; i++) {
        cost += assymmetric_distances[tour[i]][tour[i + 1]];
    }
    
    return cost;
}

float calculate_total_prize(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, int *tour) {
    float total_prize = 0.0;
    
    for (unsigned int i = 0; i < tour_size; i++) {
        int current_city = tour[i];
        for (unsigned int j = 0; j < num_all_cities; j++) {
            if (all_cities[j].index_city == (unsigned int)current_city) {
                total_prize += all_cities[j].prize;
                break;
            }
        }
    }
    
    return total_prize;
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

float calculate_objective_function(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, unsigned int prize_goal, int **assymmetric_distances, int *tour, float alpha) {
    float cost = calculate_tour_cost(tour_size, assymmetric_distances, tour);
    float total_prize = calculate_total_prize(all_cities, tour_size, num_all_cities, tour);
    float total_penalty = calculate_total_penalty(all_cities, tour_size, num_all_cities, tour);
    float relative_penalty = alpha * max(0, prize_goal - total_prize);

    if (relative_penalty > 0) {
        printf("Penalidade relativa aplicada: %.2f (Prêmio total: %.2f, Meta de prêmio: %u)\n", relative_penalty, total_prize, prize_goal);
    }

    return cost + relative_penalty + total_penalty;
}