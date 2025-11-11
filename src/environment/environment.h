#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../utils/utils.h"
#include "../reader/reader.h"

int compare_desc(const void *a, const void *b);
float initialize_alpha(int **assymmetric_distances, unsigned int num_all_cities, unsigned int n);
void init_environment(char *path_of_file, city **all_cities, unsigned int *num_all_cities, unsigned int *prize_goal, int ***assymmetric_distances,float percent_of_prize_goal);

#endif