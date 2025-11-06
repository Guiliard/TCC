#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "../../utils/utils.h"
#include "../../calculate/calculate.h"

int* copy_tour(int *tour, unsigned int tour_size);
bool is_city_in_tour(int city, int *tour, unsigned int tour_size);
int* local_search_insertion(int *current_tour, unsigned int current_tour_size, int **distances, city *cities, unsigned int num_cities, unsigned int k, float *best_fo);

#endif