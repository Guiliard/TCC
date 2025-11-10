#ifndef MIN_COST_H
#define MIN_COST_H

#include "../utils/utils.h"
#include "../calculate/calculate.h"

void convert_tour_to_min_cost(unsigned int tour_size, int **assymmetric_distances, int *tour);

#endif