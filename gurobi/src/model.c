#include <stdio.h>
#include <stdlib.h>

#include "hutil.h"
#include "gurobi_c.h"
#include "hgrb.h"

#define X_INDEX(i, j, n) ((i) * (n) + (j))
#define Y_INDEX(i, n) ((n) * (n) + (i))
#define U_INDEX(i, j, n) ((n) * (n) + (n) + ((i) * (n) + (j)))

double mGG(
    int gmin,
    int **c,
    int n,
    int *p,
    int g,
    int *w,
    double *nodect,
    char *inst,
    int *stat,
    double *rtime,
    double *gap
)
{
    (void)gmin;

    GRBenv *env = NULL;
    GRBmodel *model = NULL;

    int error = 0;
    int *ind = NULL;
    double *val = NULL;

    double fo = -1.0;
    double soma_penalidades = 0.0;
    double nodecount = -1.0;
    double runtime = -1.0;

    double mipgap = 0.0;

    int status = -1;
    int gmax = 0;
    char log_name[256];

    for (int i = 0; i < n; i++) {
        gmax += p[i];
    }

    ind = criaVetor((2 * n * n) + n);
    val = criaVetorDouble((2 * n * n) + n);

    snprintf(log_name, sizeof(log_name), "build/log/GG_%s.log", inst);

    error = GRBloadenv(&env, log_name);
    if (error) {
        goto QUIT;
    }

    error = GRBnewmodel(env, &model, inst, 0, NULL, NULL, NULL, NULL, NULL);
    if (error) {
        goto QUIT;
    }

    /*
     * Variáveis x_ij:
     * x_ij = 1 se o arco (i,j) pertence ao ciclo.
     */
    for (int i = 0; i < n; i++) {
        c[i][i] = 9999999;

        for (int j = 0; j < n; j++) {
            error = GRBaddvar(
                model,
                0,
                NULL,
                NULL,
                (double)c[i][j],
                0.0,
                1.0,
                GRB_BINARY,
                NULL
            );

            if (error) {
                goto QUIT;
            }
        }
    }

    error = GRBupdatemodel(model);
    if (error) {
        goto QUIT;
    }

    /*
     * Variáveis y_i:
     * y_i = 1 se o vértice i é visitado.
     *
     * A função objetivo original é:
     * custo das arestas + penalidade dos vértices não visitados.
     *
     * Como penalidade dos não visitados = sum(w_i) - sum(w_i y_i),
     * adicionamos sum(w_i) como constante e -w_i como coeficiente de y_i.
     */
    for (int i = 0; i < n; i++) {
        error = GRBaddvar(
            model,
            0,
            NULL,
            NULL,
            (double)-w[i],
            0.0,
            1.0,
            GRB_BINARY,
            NULL
        );

        if (error) {
            goto QUIT;
        }
    }

    /*
     * Variáveis u_ij:
     * variáveis contínuas de fluxo usadas na formulação GG.
     */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            error = GRBaddvar(
                model,
                0,
                NULL,
                NULL,
                0.0,
                0.0,
                (double)gmax,
                GRB_CONTINUOUS,
                NULL
            );

            if (error) {
                goto QUIT;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        soma_penalidades += w[i];
    }

    error = GRBsetdblattr(model, GRB_DBL_ATTR_OBJCON, soma_penalidades);
    if (error) {
        goto QUIT;
    }

    error = GRBupdatemodel(model);
    if (error) {
        goto QUIT;
    }

    /*
     * Restrição de saída:
     * sum_j x_ij = y_i
     */
    for (int i = 0; i < n; i++) {
        int k = 0;

        for (int j = 0; j < n; j++) {
            if (j != i) {
                ind[k] = X_INDEX(i, j, n);
                val[k] = 1.0;
                k++;
            }
        }

        ind[k] = Y_INDEX(i, n);
        val[k] = -1.0;
        k++;

        error = GRBaddconstr(model, k, ind, val, GRB_EQUAL, 0.0, "outOnlyOne");

        if (error) {
            goto QUIT;
        }
    }

    /*
     * Restrição de entrada:
     * sum_i x_ij = y_j
     */
    for (int j = 0; j < n; j++) {
        int k = 0;

        for (int i = 0; i < n; i++) {
            if (i != j) {
                ind[k] = X_INDEX(i, j, n);
                val[k] = 1.0;
                k++;
            }
        }

        ind[k] = Y_INDEX(j, n);
        val[k] = -1.0;
        k++;

        error = GRBaddconstr(model, k, ind, val, GRB_EQUAL, 0.0, "inOnlyOne");

        if (error) {
            goto QUIT;
        }
    }

    /*
     * Restrição de prêmio mínimo:
     * sum_i p_i y_i >= g
     *
     * O vértice 0 é tratado como raiz e não entra nessa soma.
     */
    {
        int k = 0;

        for (int i = 1; i < n; i++) {
            ind[k] = Y_INDEX(i, n);
            val[k] = (double)p[i];
            k++;
        }

        error = GRBaddconstr(model, k, ind, val, GRB_GREATER_EQUAL, (double)g, "minPrize");

        if (error) {
            goto QUIT;
        }
    }

    /*
     * A raiz deve estar na solução:
     * y_0 = 1
     */
    ind[0] = Y_INDEX(0, n);
    val[0] = 1.0;

    error = GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 1.0, "root");
    if (error) {
        goto QUIT;
    }

    /*
     * Conservação de fluxo para os vértices diferentes da raiz.
     */
    for (int i = 1; i < n; i++) {
        int k = 0;

        for (int j = 0; j < n; j++) {
            if (i != j) {
                ind[k] = U_INDEX(j, i, n);
                val[k] = 1.0;
                k++;

                ind[k] = U_INDEX(i, j, n);
                val[k] = -1.0;
                k++;
            }
        }

        ind[k] = Y_INDEX(i, n);
        val[k] = -1.0;
        k++;

        error = GRBaddconstr(model, k, ind, val, GRB_EQUAL, 0.0, "flowBalance");

        if (error) {
            goto QUIT;
        }
    }

    /*
     * Acoplamento fluxo-arco:
     * u_ij <= (n - 1) x_ij, para j != 0.
     */
    for (int i = 0; i < n; i++) {
        for (int j = 1; j < n; j++) {
            int k = 0;

            ind[k] = U_INDEX(i, j, n);
            val[k] = 1.0;
            k++;

            ind[k] = X_INDEX(i, j, n);
            val[k] = -(double)(n - 1);
            k++;

            error = GRBaddconstr(model, k, ind, val, GRB_LESS_EQUAL, 0.0, "flowArcLink");

            if (error) {
                goto QUIT;
            }
        }
    }

    error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) {
        goto QUIT;
    }

    error = GRBsetdblparam(GRBgetenv(model), GRB_DBL_PAR_TIMELIMIT, 3600.0);
    if (error) {
        goto QUIT;
    }

    error = GRBsetdblparam(GRBgetenv(model), GRB_DBL_PAR_MIPGAP, 1e-7);
    if (error) {
        goto QUIT;
    }

    error = GRBoptimize(model);
    if (error) {
        goto QUIT;
    }

    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &fo);
    if (error) {
        goto QUIT;
    }

    error = GRBgetdblattr(model, GRB_DBL_ATTR_RUNTIME, &runtime);
    if (error) {
        goto QUIT;
    }

    error = GRBgetdblattr(model, GRB_DBL_ATTR_NODECOUNT, &nodecount);
    if (error) {
        goto QUIT;
    }

    error = GRBgetdblattr(model, GRB_DBL_ATTR_MIPGAP, &mipgap);
    if (error) {
        goto QUIT;
    }

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &status);
    if (error) {
        goto QUIT;
    }

    *nodect = nodecount;
    *rtime = runtime;
    *stat = status;
    *gap = mipgap;

    {
        FILE *map = fopen("results/new_results/map_solutions.csv", "a+");

        if (map == NULL) {
            fprintf(stderr, "Erro: map_solutions.csv nao pode ser aberto.\n");
            goto QUIT;
        }

        fprintf(map, "%s;%d;%.8f;", inst, gmin, mipgap);

        for (int i = 0; i < n; i++) {
            double yi = 0.0;

            error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, Y_INDEX(i, n), &yi);
            if (error) {
                fclose(map);
                goto QUIT;
            }

            if (yi > 0.5) {
                fprintf(map, "%d ", i);
            }
        }

        fprintf(map, "\n");
        fclose(map);
    }

QUIT:
    free(ind);
    free(val);

    if (error) {
        if (env != NULL) {
            fprintf(stderr, "\nErro Gurobi: %s\n", GRBgeterrormsg(env));
        } else {
            fprintf(stderr, "\nErro Gurobi antes da criacao do ambiente.\n");
        }

        if (model != NULL) {
            GRBfreemodel(model);
        }

        if (env != NULL) {
            GRBfreeenv(env);
        }

        return -2.0;
    }

    if (model != NULL) {
        GRBfreemodel(model);
    }

    if (env != NULL) {
        GRBfreeenv(env);
    }

    return fo;
}