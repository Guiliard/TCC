#ifndef CALCULATE_H
#define CALCULATE_H

#include "../utils/utils.h"

float calculate_tour_cost(unsigned int tour_size, int **assymmetric_distances, int *tour);
float calculate_total_prize(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, int *tour);
float calculate_total_penalty(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, int *tour);
float calculate_objective_function(city *all_cities, unsigned int tour_size, unsigned int num_all_cities, unsigned int prize_goal, int **assymmetric_distances, int *tour, float alpha);

#endif