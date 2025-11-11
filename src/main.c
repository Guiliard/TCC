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
    float fo, percent_of_prize_goal = 0.2, alpha;
    city *all_cities, *initial_solution;
    int **assymmetric_distances, **symmetric_distances, *tour;
    unsigned int num_all_cities, num_initial_solution, prize_goal;

    init_environment("dataset/atex1", &all_cities, &num_all_cities, &prize_goal, &assymmetric_distances, percent_of_prize_goal);
    alpha = initialize_alpha(assymmetric_distances, num_all_cities, num_all_cities);
    build_initial_solution(&all_cities, &initial_solution, &num_all_cities, &prize_goal, &num_initial_solution);
    convert_to_symmetric(initial_solution, num_initial_solution, &assymmetric_distances, &symmetric_distances);
    print_cities(initial_solution, num_initial_solution);

    solve_tsp_with_concorde(2 * num_initial_solution, symmetric_distances, &tour, &optval);
    print_tour(2 * num_initial_solution, tour);
    convert_to_assymmetric(initial_solution, 2 * num_initial_solution, num_initial_solution, &tour);
    print_tour(num_initial_solution + 1, tour);

    convert_tour_to_min_cost(num_initial_solution + 1, assymmetric_distances, tour);
    print_tour(num_initial_solution + 1, tour);

    fo = calculate_objective_function(all_cities, num_initial_solution + 1, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    printf("Valor da função objetivo: %.2f\n", fo);

    random_insertion(all_cities, num_all_cities, num_initial_solution + 1, &tour);
    print_tour(num_initial_solution + 2, tour);

    fo = calculate_objective_function(all_cities, num_initial_solution + 2, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    printf("Valor da função objetivo após inserção aleatória: %.2f\n", fo);

    random_swap(all_cities, num_all_cities, num_initial_solution + 2, &tour);
    print_tour(num_initial_solution + 2, tour);

    fo = calculate_objective_function(all_cities, num_initial_solution + 2, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    printf("Valor da função objetivo após troca simples: %.2f\n", fo);

    random_drop(num_initial_solution + 2, &tour);
    print_tour(num_initial_solution + 1, tour);

    fo = calculate_objective_function(all_cities, num_initial_solution + 1, num_all_cities, prize_goal, assymmetric_distances, tour, alpha);
    printf("Valor da função objetivo após remoção aleatória: %.2f\n", fo);

    free(tour);
    free(initial_solution);
    free(all_cities);
    
    for (unsigned int i = 0; i < num_all_cities; i++) {
        free(assymmetric_distances[i]);
    }
    free(assymmetric_distances);
    
    for (unsigned int i = 0; i < 2 * num_initial_solution; i++) {
        free(symmetric_distances[i]);
    }
    free(symmetric_distances);
    
    return 0;
}