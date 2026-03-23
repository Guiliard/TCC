#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H

#include "../../utils/utils.h"
#include "../../solver/solver.h"
#include "../../convert/symmetric.h"
#include "../../convert/assymmetric.h"
#include "../../convert/min_cost.h"

int random_city(int num_cities);

solution* build_initial_solution(problem *prob, double *optval);

#endif