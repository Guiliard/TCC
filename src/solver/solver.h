#ifndef SOLVER_H
#define SOLVER_H

#include "../utils/utils.h"
#include "../calculate/calculate.h"
#include "../../concorde/concorde.h"
#include "../../concorde/INCLUDE/util.h"

void solve_tsp_with_concorde(solution *sol);
void solve_tsp_with_nearest_neighbor(problem *prob, solution *sol);
void solve_tsp_with_cheapest_insertion(problem *prob, solution *sol);

#endif