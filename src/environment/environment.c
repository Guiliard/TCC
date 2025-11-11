#include "environment.h"

int compare_desc(const void *a, const void *b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);
    
    if (int_a < int_b) return 1;
    if (int_a > int_b) return -1;
    return 0;
}

float initialize_alpha(int **assymmetric_distances, unsigned int num_all_cities, unsigned int n) {
    float total_sum = 0.0;
    
    for (unsigned int i = 0; i < num_all_cities; i++) {
        for (unsigned int j = 0; j < num_all_cities; j++) {
            if (i != j) {
                total_sum += assymmetric_distances[i][j];
            }
        }
    }
    
    printf("Soma total de todos os custos: %.2f\n", total_sum);
    
    unsigned int total_elements = num_all_cities * (num_all_cities - 1);
    int *all_distances = (int *)allocate_vector(sizeof(int), total_elements);
    
    unsigned int index = 0;
    for (unsigned int i = 0; i < num_all_cities; i++) {
        for (unsigned int j = 0; j < num_all_cities; j++) {
            if (i != j) {
                all_distances[index++] = assymmetric_distances[i][j];
            }
        }
    }
    
    qsort(all_distances, total_elements, sizeof(int), compare_desc);
    
    float sum_n_largest = 0.0;
    unsigned int count = (n < total_elements) ? n : total_elements;
    
    printf("As %u maiores distâncias: ", count);
    for (unsigned int i = 0; i < count; i++) {
        sum_n_largest += all_distances[i];
        printf("%d", all_distances[i]);
        if (i < count - 1) printf(", ");
    }
    printf("\n");
    
    free(all_distances);
    
    float alpha = total_sum + sum_n_largest;
    
    printf("Alpha = %.2f (soma total) + %.2f (%u maiores) = %.2f\n", total_sum, sum_n_largest, n, alpha);
    
    return alpha;
}

void init_environment(char *path_of_file, city **all_cities, unsigned int *num_all_cities, unsigned int *prize_goal, int ***assymmetric_distances, float percent_of_prize_goal) {
    char *content[] = {"_N", "_P", "_w100", "_C"};
    char filenames[4][25];

    for (unsigned int i = 0; i < 4; i++) {
        snprintf(filenames[i], sizeof(filenames[i]), "%s%s.pctsp", path_of_file, content[i]);
    }

    char *file_N = read_file(filenames[0]);
    *num_all_cities = atoi(file_N);
    free(file_N);

    city *cities_array = allocate_vector(sizeof(city), *num_all_cities);
    int **distances_matrix = allocate_matrix(*num_all_cities, *num_all_cities);

    char *file_P = read_file(filenames[1]);
    for (unsigned int i = 0; i < *num_all_cities; i++) {
        char *line = get_line_of_file(file_P, i);
        if (!line) break;
        unsigned int idx, prize;
        sscanf(line, "%u %u", &idx, &prize);
        cities_array[i].index_city = i;
        cities_array[i].prize = prize;
        free(line);
    }
    free(file_P);

    char *file_W = read_file(filenames[2]);
    for (unsigned int i = 0; i < *num_all_cities; i++) {
        char *line = get_line_of_file(file_W, i);
        if (!line) break;
        unsigned int idx, penalty;
        sscanf(line, "%u %u", &idx, &penalty);
        cities_array[i].penalty = penalty;
        free(line);
    }
    free(file_W);

    char *file_C = read_file(filenames[3]);
    unsigned int total_lines_C = count_lines(file_C);
    for (unsigned int i = 0; i < total_lines_C; i++) {
        char *line = get_line_of_file(file_C, i);
        if (!line) break;
        unsigned int from, to;
        float dist;
        sscanf(line, "%u %u %f", &from, &to, &dist);
        distances_matrix[from][to] = (int)dist;
        free(line);
    }
    free(file_C);

    unsigned int total_prize = 0;
    for (unsigned int i = 0; i < *num_all_cities; i++) {
        total_prize += cities_array[i].prize;
    }

    *prize_goal = (unsigned int)(total_prize * percent_of_prize_goal);
    *all_cities = cities_array;
    *assymmetric_distances = distances_matrix;
}