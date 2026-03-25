#include "utils.h"

void *allocate_vector(size_t element_size, size_t count) {
    void *vector = calloc(count, element_size);

    if (vector == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    return vector;
}

int **allocate_matrix(int num_rows, int num_cols)
{
    int **matrix = calloc(num_rows, sizeof(int *));

    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < num_rows; i++) {
        matrix[i] = calloc(num_cols, sizeof(int));
        if (matrix[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }

    return matrix;
}

void print_matrix(int num_rows, int num_cols, int **matrix) {
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            printf("[%d]\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_tour(int tour_size, int *tour) {
    printf("Tour: ");

    for (int i = 0; i < tour_size; i++) {
        printf("%d", tour[i]);
        if (i < tour_size - 1) {
            printf(" → ");
        }
    }

    printf("\n");
}

void print_problem(problem *prob) {
    printf("\n========================================\n");
    printf("DETALHES DO PROBLEMA\n");
    printf("========================================\n");
    printf("Cidades:\n");
    for (int i = 0; i < prob->num_all_cities; i++) {
        printf("Cidade %d: Prêmio = %d, Penalidade = %d, Distância Média = %.2f, Parâmetro = %.2f\n", prob->all_cities[i].id, prob->all_cities[i].prize, prob->all_cities[i].penalty, prob->all_cities[i].avg_distance, prob->all_cities[i].parameter);
    }
    printf("\nMatriz de Distâncias Assimétrica:\n");
    print_matrix(prob->num_all_cities, prob->num_all_cities, prob->asymmetric_distances);   
    printf("Número total de cidades: %d\n", prob->num_all_cities);
    printf("Prêmio total: %d\n", prob->total_prize);
    printf("Meta de prêmio mínimo: %d\n", prob->min_prize_goal);
    printf("Percentual do prêmio a ser alcançado: %.2f%%\n", prob->percent_of_prize * 100);
    printf("Valor de alpha: %.2f\n", prob->alpha);
}

void print_solution(solution *sol) {
    printf("\n========================================\n");
    printf("DETALHES DA SOLUÇÃO\n");
    printf("========================================\n");
    printf("Cidades visitadas:\n");
    for (int i = 0; i < sol->num_visited_cities; i++) {
        printf("Cidade %d: Prêmio = %d, Penalidade = %d, Distância Média = %.2f, Parâmetro = %.2f\n", sol->visited_cities[i].id, sol->visited_cities[i].prize, sol->visited_cities[i].penalty, sol->visited_cities[i].avg_distance, sol->visited_cities[i].parameter);
    }
    printf("Número de cidades visitadas: %d\n", sol->num_visited_cities);
    printf("Prêmio objetivo: %d\n", sol->prize_goal);
    printf("Custo total da solução: %.2f\n", sol->total_cost);
}

solution* copy_solution(solution *sol) {
    solution *new_sol = allocate_vector(sizeof(solution), 1);

    new_sol->visited_cities = allocate_vector(sizeof(city), sol->num_visited_cities);
    
    int sym_size = sol->symmetric_distances_size;
    new_sol->symmetric_distances = allocate_matrix(sym_size, sym_size);
    new_sol->symmetric_distances_size = sym_size;
    
    new_sol->tour = allocate_vector(sizeof(int), sol->tour_size);

    memcpy(new_sol->visited_cities, sol->visited_cities, sol->num_visited_cities * sizeof(city));

    for (int i = 0; i < sym_size; i++) {
        memcpy(new_sol->symmetric_distances[i], sol->symmetric_distances[i], sym_size * sizeof(int));
    }

    memcpy(new_sol->tour, sol->tour, sol->tour_size * sizeof(int));

    new_sol->tour_size = sol->tour_size;
    new_sol->num_visited_cities = sol->num_visited_cities;
    new_sol->prize_goal = sol->prize_goal;
    new_sol->total_cost = sol->total_cost;

    return new_sol;
}

void free_problem(problem *prob) {
    free(prob->all_cities);

    for (int i = 0; i < prob->num_all_cities; i++) {
        free(prob->asymmetric_distances[i]);
    }

    free(prob->asymmetric_distances);
    free(prob);
}

void free_solution(solution *sol) {
    free(sol->visited_cities);

    if(sol->symmetric_distances != NULL) {
        for (int i = 0; i < sol->symmetric_distances_size; i++) {
            free(sol->symmetric_distances[i]);
        }
        free(sol->symmetric_distances);
    }

    free(sol->tour);
    free(sol);
}