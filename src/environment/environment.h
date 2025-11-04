#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../utils/utils.h"
#include "../reader/reader.h"

void init_environment(char *path, city **cities, int ***distances, unsigned int *num_cities, float percent, unsigned int *prize_goal);

#endif