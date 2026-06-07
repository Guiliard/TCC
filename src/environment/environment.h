#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../utils/utils.h"
#include "../reader/reader.h"
#include "../calculate/calculate.h"

int compare_desc(const void *a, const void *b);

void initialize_alpha(problem *prob);
void initialize_city_parameters(problem *prob);
void initialize_ml_scores(problem *prob, char *scores_file, char *instance, int g_level);

problem* init_environment(char *number_file, char *prize_file, char* penalty_file, char *distance_file, float percent_of_prize, char *instance, int g_level);

#endif