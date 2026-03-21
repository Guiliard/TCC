#include "calculate.h"

float calculate_tour_cost(int tour_size, int **assymmetric_distances, int *tour) {
    float cost = 0.0;
    
    for (int i = 0; i < tour_size - 1; i++) {
        cost += assymmetric_distances[tour[i]][tour[i + 1]];
    }
    
    return cost;
}

float calculate_total_prize(city *all_cities, int tour_size, int num_all_cities, int *tour) {
    float total_prize = 0.0;
    
    for (int i = 0; i < tour_size; i++) {
        int current_city = tour[i];
        for (int j = 0; j < num_all_cities; j++) {
            if (all_cities[j].id == current_city) {
                total_prize += all_cities[j].prize;
                break;
            }
        }
    }
    
    return total_prize;
}


float calculate_total_penalty(city *all_cities, int tour_size, int num_all_cities, int *tour) {
    bool *visited = (bool *)allocate_vector(sizeof(bool), num_all_cities);
    float total_penalty = 0.0;
    
    for (int i = 0; i < tour_size; i++) {
        visited[tour[i]] = true;
    }
    
    for (int i = 0; i < num_all_cities; i++) {
        if (!visited[i]) {
            total_penalty += all_cities[i].penalty;
        }
    }
    
    free(visited);
    return total_penalty;
}

void calculate_objective_function(problem *prob, solution *sol) {
    float cost = calculate_tour_cost(sol->num_visited_cities + 1, prob->assymmetric_distances, sol->tour);
    float total_prize = calculate_total_prize(prob->all_cities, sol->num_visited_cities + 1, prob->num_all_cities, sol->tour);
    float total_penalty = calculate_total_penalty(prob->all_cities, sol->num_visited_cities + 1, prob->num_all_cities, sol->tour);
    float relative_penalty = prob->alpha * max(0, prob->min_prize_goal - total_prize);

    sol->total_cost = cost + relative_penalty + total_penalty;
}