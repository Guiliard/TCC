#include "min_cost.h"

void convert_tour_to_min_cost(problem *prob, solution *sol) {
    int *reverse_tour = (int *)allocate_vector(sizeof(int), sol->tour_size);
    
    for (int i = 0; i < sol->tour_size; i++) {
        reverse_tour[i] = sol->tour[sol->tour_size - 1 - i];
    }

    float cost_normal = sol->tour_cost;
    float cost_reverse = calculate_tour_cost(sol->tour_size, prob->asymmetric_distances, reverse_tour);

    printf("Tour na direção normal: %.2f", cost_normal);
    printf("\nTour na direção reversa: %.2f\n", cost_reverse);

    if (cost_reverse < cost_normal) {
        printf("Invertendo tour para direção reversa (custo: %.2f)\n", cost_reverse);
        memcpy(sol->tour, reverse_tour, sol->tour_size * sizeof(int));
        sol->tour_cost = cost_reverse;
    } else {
        printf("Mantendo tour na direção normal (custo: %.2f)\n", cost_normal);
    }
    
    free(reverse_tour);
}