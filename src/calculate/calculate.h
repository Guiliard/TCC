#ifndef CALCULATE_H
#define CALCULATE_H

#include "../utils/utils.h"

double calculate_tour_cost(int *tour, unsigned int tour_size, int **distances);
double calculate_total_penalty(int *tour, unsigned int tour_size, city *cities, unsigned int num_cities);
double calculate_objective_function(int *tour, unsigned int tour_size, int **distances, city *cities, unsigned int num_cities);

#endif