#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "../../utils/utils.h"
#include "../../calculate/calculate.h"

void local_search_insertion(int **tour, unsigned int *tour_size, int **distances, city *cities, unsigned int num_cities, float *fo);
void local_search_iterative(int **tour, unsigned int *tour_size, int **distances, city *cities, unsigned int num_cities, float *fo, unsigned int max_iterations);

#endif