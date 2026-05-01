#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H

#include "../../utils/utils.h"
#include "../../solver/solver.h"
#include "../local_search/local_search.h"

solution* grasp(problem *prob, int max_iter, float alpha, int candidate_selection_strategy);
solution* build_initial_solution_grasp(problem *prob, float alpha, int solver_type);

#endif