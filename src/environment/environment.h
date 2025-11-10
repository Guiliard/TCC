#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../utils/utils.h"
#include "../reader/reader.h"

void init_environment(char *path_of_file, city **all_cities, unsigned int *num_all_cities, unsigned int *prize_goal, int ***assymmetric_distances,float percent_of_prize_goal);

#endif