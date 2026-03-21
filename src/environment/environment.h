#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../utils/utils.h"
#include "../reader/reader.h"

int compare_desc(const void *a, const void *b);
void initialize_alpha(problem *prob);
problem* init_environment(char *number_file, char *prize_file, char* penalty_file, char *distance_file);

#endif