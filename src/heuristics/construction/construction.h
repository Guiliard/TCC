#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H

#include "../../utils/utils.h"
#include "../../solver/solver.h"
#include "../../convert/min_cost.h"
#include "../../convert/symmetric.h"
#include "../../convert/asymmetric.h"
#include "../local_search/local_search.h"

void resolve_tsp_with_concorde(problem* prob, solution *sol);

solution* grasp(problem *prob, int max_iter, float alpha);
solution* build_initial_solution_grasp(problem *prob, float alpha);

#endif