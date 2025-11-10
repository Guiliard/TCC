#include "construction.h"

unsigned int random_city(unsigned int num_cities) {
    return (rand() % (num_cities - 1)) + 1;
}

void build_initial_solution(city **all_cities, city **initial_solution, unsigned int *num_cities, unsigned int *prize_goal, unsigned int *num_initial_solution) {
    unsigned int total_prize = 0, count = 1,capacity = *prize_goal;
    bool *visited = allocate_vector(sizeof(bool), *num_cities);
    city *temp_solution = allocate_vector(sizeof(city), *num_cities);

    temp_solution[0] = (*all_cities)[0];

    while (count < *num_cities) {
        unsigned int idx = random_city(*num_cities);
        if(!visited[idx]) {
            city selected_city = (*all_cities)[idx];
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

    *initial_solution = solution;
    *num_initial_solution = count;
}