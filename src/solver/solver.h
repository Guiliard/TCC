#ifndef SOLVER_H
#define SOLVER_H

#include "../utils/utils.h"
#include "../../concorde/concorde.h"
#include "../../concorde/INCLUDE/util.h"

int* solve_tsp_with_concorde(int **symmetric_distances, unsigned int num_nodes, double *optval);

#endif