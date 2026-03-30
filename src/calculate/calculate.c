#include "calculate.h"

float calculate_greedy_value(problem *prob, int last_city, int candidate_city) {
    float distance = prob->asymmetric_distances[last_city][candidate_city];

    city *c = &prob->all_cities[candidate_city];

    return (c->prize + c->penalty) / distance;
}

float calculate_average_distance(problem *prob, int city_index) {
    float sum = 0.0;

    for (int j = 0; j < prob->num_all_cities; j++) {
        if (j != city_index) {
            sum += prob->asymmetric_distances[city_index][j];
        }
    }

    return sum / (prob->num_all_cities - 1);
}

float calculate_city_parameter(problem *prob, int city_index) {
    city *c = &prob->all_cities[city_index];

    c->avg_distance = calculate_average_distance(prob, city_index);

    c->parameter = (c->prize * c->penalty) / c->avg_distance;

    return c->parameter;
}

float calculate_tour_cost(int tour_size, int **asymmetric_distances, int *tour) {
    float cost = 0.0;
    
    for (int i = 0; i < tour_size - 1; i++) {
        cost += asymmetric_distances[tour[i]][tour[i + 1]];
    }
    
    return cost;
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
    float total_penalty = calculate_total_penalty(prob->all_cities, sol->tour_size, prob->num_all_cities, sol->tour);
    float relative_penalty = prob->alpha * max(0, prob->min_prize_goal - sol->prize_goal);

    sol->total_cost = sol->tour_cost + total_penalty + relative_penalty;
}