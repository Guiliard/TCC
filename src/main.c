#include "environment/environment.h"
#include "heuristics/construction/construction.h"
#include "heuristics/local_search/local_search.h"
#include "convert/symmetric.h"
#include "convert/assymmetric.h"
#include "convert/min_cost.h"
#include "solver/solver.h"

int main() {
    srand(time(NULL));

    double optval;
    problem* prob = NULL;
    solution* sol = NULL;

    prob = init_environment(
        "dataset/atex1_N.pctsp", 
        "dataset/atex1_P.pctsp", 
        "dataset/atex1_w100.pctsp", 
        "dataset/atex1_C.pctsp"
    );

    sol = build_initial_solution(
        prob, 
        &optval
    );

    // random_insertion(all_cities, num_all_cities, num_initial_solution + 1, &tour);
    // print_tour(num_initial_solution + 2, tour);

    // fo = calculate_objective_function(all_cities, num_initial_solution + 2, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    // printf("Valor da função objetivo após inserção aleatória: %.2f\n", fo);

    // random_swap(all_cities, num_all_cities, num_initial_solution + 2, &tour);
    // print_tour(num_initial_solution + 2, tour);

    // fo = calculate_objective_function(all_cities, num_initial_solution + 2, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    // printf("Valor da função objetivo após troca simples: %.2f\n", fo);

    // random_drop(num_initial_solution + 2, &tour);
    // print_tour(num_initial_solution + 1, tour);

    // fo = calculate_objective_function(all_cities, num_initial_solution + 1, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    // printf("Valor da função objetivo após remoção aleatória: %.2f\n", fo);

    // best_swap(all_cities, num_all_cities, num_initial_solution + 1, &tour, assymmetric_distances, alpha, prize_goal);
    // print_tour(num_initial_solution + 1, tour);

    // fo = calculate_objective_function(all_cities, num_initial_solution + 1, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    // printf("Valor da função objetivo após melhor troca: %.2f\n", fo);

    // free(tour);
    // free(initial_solution);
    // free(all_cities);
    
    // for (unsigned int i = 0; i < num_all_cities; i++) {
    //     free(assymmetric_distances[i]);
    // }
    // free(assymmetric_distances);
    
    // for (unsigned int i = 0; i < 2 * num_initial_solution; i++) {
    //     free(symmetric_distances[i]);
    // }
    // free(symmetric_distances);
    
    return 0;
}