#include "construction.h"

int random_city(int num_cities) {
    return (rand() % (num_cities - 1)) + 1;
}

solution* build_initial_solution(problem *prob, double *optval) {
    solution* sol = allocate_vector(sizeof(solution), 1);
    
    int total_prize = 0;
    int count = 1;
    int capacity = prob->min_prize_goal;

    bool *visited = allocate_vector(sizeof(bool), prob->num_all_cities);
    city *solution = allocate_vector(sizeof(city), prob->num_all_cities);

    solution[0] = prob->all_cities[0];

    while (count < prob->num_all_cities) {
        int idx = random_city(prob->num_all_cities);

        if (!visited[idx]) {
            city selected_city = prob->all_cities[idx];

            solution[count] = selected_city;
            total_prize += selected_city.prize;

            visited[idx] = true;
            count++;

            if (total_prize >= capacity) {
                break;
            }
        }
    }

    solution = realloc(solution, count * sizeof(city));

    sol->visited_cities = solution;
    sol->num_visited_cities = count;
    sol->prize_goal = total_prize;

    free(visited);

    convert_to_symmetric(prob, sol);
    solve_tsp_with_concorde(sol, optval);
    print_tour(sol->tour_size, sol->tour);
    convert_to_assymmetric(sol);
    print_tour(sol->tour_size, sol->tour);
    convert_tour_to_min_cost(prob, sol);
    print_tour(sol->tour_size, sol->tour);
    calculate_objective_function(prob, sol);

    return sol;
}