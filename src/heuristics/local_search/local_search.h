#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "../../utils/utils.h"

bool is_city_in_tour(int city, int *tour, unsigned int tour_size);
void random_insertion(city *all_cities, unsigned int num_all_cities, unsigned int tour_size, int **tour);
void random_swap(city *all_cities, unsigned int num_all_cities, unsigned int tour_size, int **tour);
void random_drop(unsigned int tour_size, int **tour);

#endif