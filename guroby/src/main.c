#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hutil.h"
#include "hgrb.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <2|5|7|8|1> <instancia>\n", argv[0]);
        return 1;
    }

    double gmin = 0.0;
    double soma = 0.0;
    int g = 0;
    char pname[256];
    char *fname = argv[2];

    if (strcmp(argv[1], "2") == 0) {
        gmin = 0.2;
    } else if (strcmp(argv[1], "5") == 0) {
        gmin = 0.5;
    } else if (strcmp(argv[1], "7") == 0) {
        gmin = 0.75;
    } else if (strcmp(argv[1], "8") == 0) {
        gmin = 0.8;
    } else if (strcmp(argv[1], "1") == 0) {
        gmin = 1.0;
    } else {
        fprintf(stderr, "Erro: valor invalido para gmin: %s\n", argv[1]);
        return 1;
    }

    printf("\n-----------------------------------------------------------\n");
    printf(" -Prob inst.: %s\n", fname);

    snprintf(pname, sizeof(pname), "../dataset/%s/%s_N.pctsp", fname, fname);
    int n = carregaParN(pname);

    int *p = criaVetor(n);
    int *w = criaVetor(n);
    int **c = criaMatriz(n, n);

    snprintf(pname, sizeof(pname), "../dataset/%s/%s_C.pctsp", fname, fname);
    carregaMatriz(pname, c);

    snprintf(pname, sizeof(pname), "../dataset/%s/%s_P.pctsp", fname, fname);
    carregaVetor(pname, p);

    snprintf(pname, sizeof(pname), "../dataset/%s/%s_w100.pctsp", fname, fname);
    carregaVetor(pname, w);

    for (int i = 0; i < n; i++) {
        soma += p[i];
    }

    g = (int)(gmin * soma + 0.5);

    solverGRB((int)(gmin * 10), c, n, p, g, w, fname);

    liberaMatriz(c, n);
    free(p);
    free(w);

    printf(" -FIM\n-----------------------------------------------------------\n");

    return 0;
}