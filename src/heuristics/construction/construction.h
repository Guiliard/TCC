#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H

#include "../../utils/utils.h"

unsigned int random_city(unsigned int num_cities);
void build_initial_solution(city **all_cities, city **initial_solution, unsigned int *num_all_cities, unsigned int *prize_goal, unsigned int *num_initial_solution);

#endif