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
    float fo, percent = 0.2;
    city *cities, *initial_solution;
    int **distances, **symmetric_distances, *tour = NULL, *original_tour = NULL;
    unsigned int num_cities, num_initial_solution, num_original_tour, prize_goal;

    init_environment("dataset/atex1", &cities, &distances, &num_cities, percent, &prize_goal);
    build_initial_solution(&cities, &num_cities, &prize_goal, &initial_solution, &num_initial_solution, &num_original_tour);
    convert_to_symmetric(&distances, initial_solution, num_initial_solution, &symmetric_distances);
    print_cities(initial_solution, num_initial_solution);
    tour = solve_tsp_with_concorde(symmetric_distances, 2 * num_initial_solution, &optval);
    print_tour(tour, 2 * num_initial_solution);

    if (tour != NULL) {
        original_tour = convert_to_assymmetric(tour, 2 * num_initial_solution, num_initial_solution, initial_solution);
        print_tour(original_tour, num_original_tour);
        if (original_tour != NULL) {
            convert_tour_to_min_cost(original_tour, num_original_tour, distances);
            print_tour(original_tour, num_original_tour);
            fo = calculate_objective_function(original_tour, num_original_tour, distances, cities, num_cities);
            printf("Valor da função objetivo: %.2f\n", fo);
            free(original_tour);
        }
        free(tour);
    }
    
    return 0;
}