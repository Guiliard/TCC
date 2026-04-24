#ifndef HGRB_H
#define HGRB_H

#include "gurobi_c.h"

void solverGRB(int gmin, int **c, int n, int *p, int g, int *w, char *nameINST);

double mGG(
	int gmin,
	int **c,
	int n,
	int *p,
	int g,
	int *w,
	double *nodect,
	char *inst,
	int *stat,
	double *rtime
);

#endif