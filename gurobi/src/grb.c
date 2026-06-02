#include "hutil.h"
#include "hgrb.h"

void solverGRB(int gmin, int **c, int n, int *p, int g, int *w, char *nameINST)
{
    double fostar = -1.0;
    double gap = 0.0;
    double nodect = 0.0;
    int status = 0;
    double runtime = 0.0;

    fostar = mGG(gmin, c, n, p, g, w, &nodect, nameINST, &status, &runtime, &gap);

    imprimRel(fostar, nameINST, "GG", gap, nodect, gmin, status, runtime);
}