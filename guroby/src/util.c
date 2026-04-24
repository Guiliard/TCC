#include <stdio.h>
#include <stdlib.h>

#include "hutil.h"

int *criaVetor(int tam)
{
    int *vetor = malloc((size_t)tam * sizeof(int));

    if (vetor == NULL) {
        fprintf(stderr, "Erro: falta memoria para alocar vetor int.\n");
        exit(1);
    }

    return vetor;
}

double *criaVetorDouble(int tam)
{
    double *vetor = malloc((size_t)tam * sizeof(double));

    if (vetor == NULL) {
        fprintf(stderr, "Erro: falta memoria para alocar vetor double.\n");
        exit(1);
    }

    return vetor;
}

int **criaMatriz(int nlinhas, int ncolunas)
{
    int **matriz = malloc((size_t)nlinhas * sizeof(int *));

    if (matriz == NULL) {
        fprintf(stderr, "Erro: falta memoria para alocar matriz.\n");
        exit(1);
    }

    for (int i = 0; i < nlinhas; i++) {
        matriz[i] = malloc((size_t)ncolunas * sizeof(int));

        if (matriz[i] == NULL) {
            fprintf(stderr, "Erro: falta memoria para alocar linha da matriz.\n");
            exit(1);
        }
    }

    return matriz;
}

void iniciaVetor(int *vetor, int tam, int v)
{
    for (int i = 0; i < tam; i++) {
        vetor[i] = v;
    }
}

void iniciaVetorDouble(double *vetor, int tam, double v)
{
    for (int i = 0; i < tam; i++) {
        vetor[i] = v;
    }
}

void liberaMatriz(int **matriz, int nlinhas)
{
    for (int i = 0; i < nlinhas; i++) {
        free(matriz[i]);
    }

    free(matriz);
}

int carregaParN(char nomearq[])
{
    FILE *arquivo = fopen(nomearq, "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: arquivo %s nao pode ser aberto.\n", nomearq);
        exit(1);
    }

    int n = 0;

    if (fscanf(arquivo, "%d", &n) != 1) {
        fprintf(stderr, "Erro: falha ao ler n em %s.\n", nomearq);
        fclose(arquivo);
        exit(1);
    }

    fclose(arquivo);

    return n;
}

void carregaMatriz(char nomearq[], int **matriz)
{
    FILE *arquivo = fopen(nomearq, "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: arquivo %s nao pode ser aberto.\n", nomearq);
        exit(1);
    }

    int i, j, valor;

    while (fscanf(arquivo, "%d %d %d", &i, &j, &valor) == 3) {
        matriz[i][j] = valor;
    }

    fclose(arquivo);
}

void carregaVetor(char nomearq[], int *vetor)
{
    FILE *arquivo = fopen(nomearq, "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: arquivo %s nao pode ser aberto.\n", nomearq);
        exit(1);
    }

    int i, valor;

    while (fscanf(arquivo, "%d %d", &i, &valor) == 2) {
        vetor[i] = valor;
    }

    fclose(arquivo);
}

void imprimRel(
    double fostar,
    const char *nameINST,
    const char *nameFORM,
    double gap,
    double nodect,
    int g,
    int status,
    double runtime
)
{
    const char *cond;

    switch (status) {
        case 1: cond = "LOADED"; break;
        case 2: cond = "OPTIMAL"; break;
        case 3: cond = "INFEASIBLE"; break;
        case 4: cond = "INF_OR_UNBD"; break;
        case 5: cond = "UNBOUNDED"; break;
        case 7: cond = "ITERATION_LIMIT"; break;
        case 8: cond = "NODE_LIMIT"; break;
        case 9: cond = "TIME_LIMIT"; break;
        case 11: cond = "INTERRUPTED"; break;
        case 12: cond = "NUMERIC"; break;
        case 13: cond = "SUBOPTIMAL"; break;
        case 10001: cond = "OUT_OF_MEMORY"; break;
        case 20003: cond = "Internal_Gurobi_error"; break;
        default: cond = "ERROR_UNKNOWN"; break;
    }

    FILE *arquivo = fopen("Relatorio.rel", "a+");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Relatorio.rel nao pode ser aberto.\n");
        exit(1);
    }

    fprintf(
        arquivo,
        "%s, %d, %s, %.1f, %.2f, %.0f, %s, %.1f\n",
        nameINST,
        g,
        nameFORM,
        fostar,
        gap,
        nodect,
        cond,
        runtime
    );

    fclose(arquivo);
}