#include "environment/environment.h"
#include "heuristics/construction/construction.h"
#include "metrics/metrics.h"

int main(int argc, char *argv[]) {
    char instance[INSTANCE_NAME_SIZE] = "rbg403";
    float alpha = 0.3f;
    int max_iter = 100;
    int selection = CANDIDATE_SELECTION_ORDERED;
    int g_level = 2;
    float percent_of_prize = 0.2f;
    unsigned int seed = 12345;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--instance") == 0 && i + 1 < argc) {
            strncpy(instance, argv[++i], INSTANCE_NAME_SIZE - 1);
            instance[INSTANCE_NAME_SIZE - 1] = '\0';
        }
        else if (strcmp(argv[i], "--alpha") == 0 && i + 1 < argc) {
            alpha = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--max-iter") == 0 && i + 1 < argc) {
            max_iter = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--selection") == 0 && i + 1 < argc) {
            char *value = argv[++i];

            if (strcmp(value, "ordered") == 0) {
                selection = CANDIDATE_SELECTION_ORDERED;
            }
            else if (strcmp(value, "random") == 0) {
                selection = CANDIDATE_SELECTION_RANDOM;
            }
            else if (strcmp(value, "roulette") == 0) {
                selection = CANDIDATE_SELECTION_ROULETTE;
            }
            else {
                fprintf(stderr, "Erro: valor invalido para --selection: %s\n", value);
                return 1;
            }
        }
        else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int) atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--g") == 0 && i + 1 < argc) {
            g_level = atoi(argv[++i]);

            if (g_level == 2) {
                percent_of_prize = 0.2f;
            }
            else if (g_level == 5) {
                percent_of_prize = 0.5f;
            }
            else if (g_level == 8) {
                percent_of_prize = 0.8f;
            }
            else {
                fprintf(stderr, "Erro: --g deve ser 2, 5 ou 8\n");
                return 1;
            }
        }
        else {
            fprintf(stderr, "Argumento invalido: %s\n", argv[i]);
            return 1;
        }
    }

    char n_file[512];
    char p_file[512];
    char w_file[512];
    char c_file[512];

    snprintf(n_file, sizeof(n_file), "dataset/%s/%s_N.pctsp", instance, instance);
    snprintf(p_file, sizeof(p_file), "dataset/%s/%s_P.pctsp", instance, instance);
    snprintf(w_file, sizeof(w_file), "dataset/%s/%s_w100.pctsp", instance, instance);
    snprintf(c_file, sizeof(c_file), "dataset/%s/%s_C.pctsp", instance, instance);

    if (alpha < 0.0f || alpha > 1.0f) {
        fprintf(stderr, "Erro: alpha deve estar entre 0 e 1\n");
        return 1;
    }

    if (max_iter <= 0) {
        fprintf(stderr, "Erro: max_iter deve ser maior que zero\n");
        return 1;
    }

    srand(seed);

    problem* prob = NULL;
    solution* sol = NULL;
    metrics* m = create_metrics();

    prob = init_environment(n_file, p_file, w_file, c_file, percent_of_prize);

    start_metrics(m);

    sol = grasp(prob, max_iter, alpha, selection);

    stop_metrics(m);

    printf("%.2f\n", sol->total_cost);
    printf("%.2f\n", m->elapsed_time);
    // print_report(instance, prob, sol, m->elapsed_time, alpha, max_iter, selection);

    free_problem(prob);
    free_solution(sol);
    free_metrics(m);

    return 0;
}