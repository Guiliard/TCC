#include "calculate.h"

float calculate_average_distance(problem *prob, int city_index) {
    float outgoing_sum = 0.0;
    float incoming_sum = 0.0;

    for (int j = 0; j < prob->num_all_cities; j++) {
        if (j != city_index) {
            outgoing_sum += prob->asymmetric_distances[city_index][j];
            incoming_sum += prob->asymmetric_distances[j][city_index];
        }
    }

    float outgoing_avg = outgoing_sum / (prob->num_all_cities - 1);
    float incoming_avg = incoming_sum / (prob->num_all_cities - 1);

    return (outgoing_avg + incoming_avg) / 2.0;
}

float calculate_city_parameter(problem *prob, int city_index) {
    city *c = &prob->all_cities[city_index];

    c->avg_distance = calculate_average_distance(prob, city_index);

    c->parameter = (c->prize * c->penalty) / c->avg_distance;

    return c->parameter;
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

double calculate_tour_cost(int tour_size, int **asymmetric_distances, int *tour) {
    double cost = 0.0;
    
    for (int i = 0; i < tour_size - 1; i++) {
        cost += asymmetric_distances[tour[i]][tour[i + 1]];
    }
    
    return cost;
}

void calculate_objective_function(problem *prob, solution *sol) {
    sol->tour_cost = calculate_tour_cost(sol->tour_size, prob->asymmetric_distances, sol->tour);
    
    float total_penalty = calculate_total_penalty(prob->all_cities, sol->tour_size, prob->num_all_cities, sol->tour);
    float relative_penalty = prob->alpha * max(0, prob->min_prize_goal - sol->prize_goal);

    sol->total_cost = sol->tour_cost + total_penalty + relative_penalty;
}