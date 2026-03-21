#include "construction.h"

int random_city(int num_cities) {
    return (rand() % (num_cities - 1)) + 1;
}

solution* build_initial_solution(problem *prob, double *optval) {
    solution* sol = allocate_vector(sizeof(solution), 1);
    int total_prize = 0, count = 1, capacity = (prob->total_prize * prob->percent_of_prize);
    bool *visited = allocate_vector(sizeof(bool), prob->num_all_cities);
    city *temp_solution = allocate_vector(sizeof(city), prob->num_all_cities);

    temp_solution[0] = prob->all_cities[0];

    while (count < prob->num_all_cities) {
        int idx = random_city(prob->num_all_cities);
        if(!visited[idx]) {
            city selected_city = prob->all_cities[idx];
            temp_solution[count] = selected_city;
            total_prize += selected_city.prize;
            count++;
            visited[idx] = true;
            if (total_prize >= capacity) {
                break;
            }
        }
    }

    city *solution = allocate_vector(sizeof(city), count);
    memcpy(solution, temp_solution, count * sizeof(city));
    free(temp_solution);

    sol->visited_cities = solution;
    sol->num_visited_cities = count;
    sol->prize_goal = total_prize;

    convert_to_symmetric(prob, sol);
    solve_tsp_with_concorde(sol, optval);
    convert_to_assymmetric(sol);
    convert_tour_to_min_cost(prob, sol);
    calculate_objective_function(prob, sol);

    return sol;
}