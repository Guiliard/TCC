#include "environment/environment.h"
#include "heuristics/construction/construction.h"

int main() {
    srand(time(NULL));

    problem* prob = NULL;
    solution* sol = NULL;

    prob = init_environment(
        "dataset/atex1_N.pctsp", 
        "dataset/atex1_P.pctsp", 
        "dataset/atex1_w100.pctsp", 
        "dataset/atex1_C.pctsp"
    );

    print_problem(prob);

    sol = grasp(
        prob,
        MAX_ITER,
        ALPHA
    );

    print_solution(sol);
    
    return 0;
}