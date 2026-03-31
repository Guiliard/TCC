#include "environment/environment.h"
#include "heuristics/construction/construction.h"
#include "metrics/metrics.h"

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

    metrics* m = create_metrics();
    start_metrics(m);

    sol = grasp(
        prob,
        MAX_ITER,
        ALPHA
    );

    stop_metrics(m);

    print_solution(sol);
    
    print_report(
        "atex1",
        prob,
        sol,
        m->elapsed_time,
        ALPHA,
        MAX_ITER,
        CANDIDATE_SELECTION_ORDERED
    );

    free_problem(prob);
    free_solution(sol);
    free_metrics(m);
    
    return 0;
}