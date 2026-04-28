#include "environment.h"

int compare_desc(const void *a, const void *b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);
    
    if (int_a < int_b) return 1;
    if (int_a > int_b) return -1;
    return 0;
}

void initialize_city_parameters(problem *prob) {
    for (int i = 0; i < prob->num_all_cities; i++) {
        calculate_city_parameter(prob, i);
    }
}

void initialize_alpha(problem *prob) {
    float total_sum = 0.0;
    
    for (int i = 0; i < prob->num_all_cities; i++) {
        for (int j = 0; j < prob->num_all_cities; j++) {
            if (i != j) {
                total_sum += prob->asymmetric_distances[i][j];
            }
        }
    }

    int total_elements = prob->num_all_cities * (prob->num_all_cities - 1);
    int *all_distances = (int *)allocate_vector(sizeof(int), total_elements);
    
    int index = 0;
    for (int i = 0; i < prob->num_all_cities; i++) {
        for (int j = 0; j < prob->num_all_cities; j++) {
            if (i != j) {
                all_distances[index++] = prob->asymmetric_distances[i][j];
            }
        }
    }
    
    qsort(all_distances, total_elements, sizeof(int), compare_desc);
    
    float sum_n_largest = 0.0;
    int count = (prob->num_all_cities < total_elements) ? prob->num_all_cities : total_elements;

    for (int i = 0; i < count; i++) {
        sum_n_largest += all_distances[i];
    }
    
    free(all_distances);
    
    prob->alpha = total_sum + sum_n_largest;
}

problem* init_environment(char *number_file, char *prize_file, char* penalty_file, char *distance_file, float percent_of_prize) {
    problem* prob = allocate_vector(sizeof(problem), 1);

    char* number_content = read_file((char*)number_file);
    char* prize_content = read_file((char*)prize_file);
    char* penalty_content = read_file((char*)penalty_file);
    char* distance_content = read_file((char*)distance_file);

    prob->total_prize = 0;
    prob->percent_of_prize = percent_of_prize;
    prob->num_all_cities = atoi(number_content);
    prob->all_cities = allocate_vector(sizeof(city), prob->num_all_cities);
    prob->asymmetric_distances = allocate_matrix(prob->num_all_cities, prob->num_all_cities);

    for (int i = 0; i < prob->num_all_cities; i++) {
        char *line = get_line_of_file(prize_content, i);
        sscanf(line, "%d %d", &prob->all_cities[i].id, &prob->all_cities[i].prize);
        free(line);
        prob->total_prize += prob->all_cities[i].prize;
    }

    for (int i = 0; i < prob->num_all_cities; i++) {
        char *line = get_line_of_file(penalty_content, i);
        sscanf(line, "%*d %d", &prob->all_cities[i].penalty);
        free(line);
    }

    int total_lines_distance = count_lines(distance_content);
    for (int i = 0; i < total_lines_distance; i++) {
        int from, to, dist;
        char *line = get_line_of_file(distance_content, i);
        sscanf(line, "%d %d %d", &from, &to, &dist);
        prob->asymmetric_distances[from][to] = dist;
        free(line);
    }

    prob->min_prize_goal = (int)(prob->total_prize * prob->percent_of_prize);

    free(number_content);
    free(prize_content);
    free(penalty_content);
    free(distance_content);

    initialize_alpha(prob);
    initialize_city_parameters(prob);

    return prob;
}