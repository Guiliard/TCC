#ifndef CALCULATE_H
#define CALCULATE_H

#include "../utils/utils.h"

float calculate_tour_cost(int *tour, unsigned int tour_size, int **distances);
float calculate_total_penalty(int *tour, unsigned int tour_size, city *cities, unsigned int num_cities);
float calculate_objective_function(int *tour, unsigned int tour_size, int **distances, city *cities, unsigned int num_cities);

#endif