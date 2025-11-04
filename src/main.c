#include "environment/environment.h"
#include "heuristics/construction/construction.h"
#include "heuristics/local_search/local_search.h"
#include "convert/symmetric.h"
#include "convert/assymmetric.h"
#include "solver/solver.h"
#include "calculate/calculate.h"

int main() {
    srand(time(NULL));

    double optval;
    float fo, percent = 0.2;
    city *cities, *initial_solution;
    int **distances, **symmetric_distances, *tour = NULL, *original_tour = NULL;
    unsigned int num_cities, num_initial_solution, prize_goal, inserted_cities = 3;

    init_environment("dataset/atex1", &cities, &distances, &num_cities, percent, &prize_goal);
    build_initial_solution(&cities, &num_cities, &prize_goal, &initial_solution, &num_initial_solution);
    convert_to_symmetric(&distances, initial_solution, num_initial_solution, &symmetric_distances);
    print_cities(initial_solution, num_initial_solution);
    print_matrix(symmetric_distances, 2 * num_initial_solution, 2 * num_initial_solution);
    tour = solve_tsp_with_concorde(symmetric_distances, 2 * num_initial_solution, &optval);

    if (tour != NULL) {
        original_tour = convert_to_assymmetric(tour, 2 * num_initial_solution, num_initial_solution, initial_solution);
        if (original_tour != NULL) {
            fo = calculate_objective_function(original_tour, num_initial_solution, distances, cities, num_cities);
            printf("Valor da função objetivo: %.2f\n", fo);
            local_search_iterative(&original_tour, &num_initial_solution, distances, cities, num_cities, &fo, inserted_cities);
            printf("Valor da função objetivo após busca local: %.2f\n", fo);
            free(original_tour);
        }
        free(tour);
    }
    
    return 0;
}