#ifndef SOLVER_H
#define SOLVER_H

#include "../utils/utils.h"
#include "../../concorde/concorde.h"
#include "../../concorde/INCLUDE/util.h"

void solve_tsp_with_concorde(unsigned int num_nodes, int **symmetric_distances, int** tour, double *optval);

#endif