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
    print_tour(sol->tour_size, sol->tour);
    printf("Número de cidades visitadas: %d\n", sol->num_visited_cities);
    printf("Prêmio objetivo: %d\n", sol->prize_goal);
    printf("Custo total da solução: %.2f\n", sol->total_cost);
    printf("Custo da tour: %.2f\n", sol->tour_cost);
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

void print_report(const char *instance_name, problem *prob, solution *sol, double execution_time, float alpha, int num_iterations, int candidate_selection_type)
{
    const char *selection_type_str = "UNKNOWN";
    
    switch (candidate_selection_type) {
        case CANDIDATE_SELECTION_ORDERED:
            selection_type_str = "ORDERED";
            break;
        case CANDIDATE_SELECTION_RANDOM:
            selection_type_str = "RANDOM";
            break;
        case CANDIDATE_SELECTION_ROULETTE:
            selection_type_str = "ROULETTE";
            break;
    }

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                  RELATÓRIO DE EXECUÇÃO - PCTSP                 ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  Instância:                       %s\n", instance_name);
    printf("  Prêmio Mínimo:                   %d\n", prob->min_prize_goal);
    printf("  Prêmio Coletado:                 %d\n", sol->prize_goal);
    printf("  Tempo de Execução:               %.6f segundos\n", execution_time);
    printf("  Função Objetivo (FO):            %.2f\n", sol->total_cost);
    printf("  Alpha (GRASP):                   %.4f\n", alpha);
    printf("  Número de Iterações:             %d\n", num_iterations);
    printf("  Tipo de Seleção:                 %s\n", selection_type_str);
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("\n");
}