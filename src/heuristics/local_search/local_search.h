#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "../../utils/utils.h"
#include "../../solver/solver.h"
#include "../../convert/min_cost.h"
#include "../../convert/symmetric.h"
#include "../../convert/asymmetric.h"
#include "../../calculate/calculate.h"

int compare_parameter_asc(const void *a, const void *b);
int compare_parameter_desc(const void *a, const void *b);

bool try_solution(problem *prob, solution *sol, float original_cost);

void shuffle_array(int *array, int size);
void roulette_select(int *candidates, int num_candidates);
void select_candidates(int *candidates, int num_candidates, int selection, int (*compar)(const void*, const void*));
void insertion_move(problem* prob, solution* sol, int selection);
void drop_move(problem* prob, solution* sol, int selection);
void swap_move(problem* prob, solution* sol, int selection);
void vnd(problem* prob, solution* sol, int selection);

#endif