#ifndef SOLVER_H
#define SOLVER_H

#include "../utils/utils.h"
#include "../calculate/calculate.h"
#include "../../concorde/concorde.h"
#include "../../concorde/INCLUDE/util.h"
#include "../convert/symmetric.h"
#include "../convert/asymmetric.h"
#include "../heuristics/local_search/local_search.h"

int *build_initial_symmetric_tour_from_solution(solution *sol);
int solve_tsp_with_concorde(solution *sol);

void solve_tsp_with_nearest_neighbor(problem *prob, solution *sol);
void solve_tsp_with_cheapest_insertion(problem *prob, solution *sol);

#endif