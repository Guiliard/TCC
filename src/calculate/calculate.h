#ifndef CALCULATE_H
#define CALCULATE_H

#include "../utils/utils.h"

float calculate_average_distance(problem *prob, int city_index);
float calculate_city_parameter(problem *prob, int city_index);
float calculate_tour_cost(int tour_size, int **assymmetric_distances, int *tour);
float calculate_total_penalty(city *all_cities,  int tour_size, int num_all_cities, int *tour);

void calculate_objective_function(problem *prob, solution *sol);

#endif