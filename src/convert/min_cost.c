#include "min_cost.h"

void convert_tour_to_min_cost(unsigned int tour_size, int **assymmetric_distances, int *tour) {
    int *reverse_tour = (int *)allocate_vector(sizeof(int), tour_size);
    for (unsigned int i = 0; i < tour_size; i++) {
        reverse_tour[i] = tour[tour_size - 1 - i];
    }

    float cost_normal = calculate_tour_cost(tour_size, assymmetric_distances, tour);
    float cost_reverse = calculate_tour_cost(tour_size, assymmetric_distances, reverse_tour);

    printf("Tour na direção normal: %.2f", cost_normal);
    printf("\nTour na direção reversa: %.2f\n", cost_reverse);

    if (cost_reverse < cost_normal) {
        printf("Invertendo tour para direção reversa (custo: %.2f)\n", cost_reverse);
        memcpy(tour, reverse_tour, tour_size * sizeof(int));
    } else {
        printf("Mantendo direção normal (custo: %.2f)\n", cost_normal);
    }
    
    free(reverse_tour);
}