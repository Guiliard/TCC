#include "utils.h"

void *allocate_vector(size_t element_size, size_t count) {
    void *vector = calloc(count, element_size);
    if (vector == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    return vector;
}

int **allocate_matrix(unsigned int num_rows, unsigned int num_cols)
{
    int **matrix = calloc(num_rows, sizeof(int *));
    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for (unsigned int i = 0; i < num_rows; i++) {
        matrix[i] = calloc(num_cols, sizeof(float));
        if (matrix[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }
    return matrix;
}

void print_cities(city *cities, unsigned int num_cities) {
    for (unsigned int i = 0; i < num_cities; i++) {
        printf("City %u: Prize = %u, Penalty = %u\n", cities[i].index_city, cities[i].prize, cities[i].penalty);
    }
    printf("\n");
}

void print_matrix(int **matrix, unsigned int num_rows, unsigned int num_cols) {
    for (unsigned int i = 0; i < num_rows; i++) {
        for (unsigned int j = 0; j < num_cols; j++) {
            printf("[%d]\t", matrix[i][j]);
        }
        printf("\n");
    }
}