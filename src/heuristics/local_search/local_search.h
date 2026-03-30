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

void insertion_move(problem* prob, solution* sol);
void drop_move(problem* prob, solution* sol);
void swap_move(problem* prob, solution* sol);
void vnd(problem* prob, solution* sol);

#endif