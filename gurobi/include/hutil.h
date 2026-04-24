#ifndef HUTIL_H
#define HUTIL_H

int *criaVetor(int tam);
double *criaVetorDouble(int tam);
int **criaMatriz(int nlinhas, int ncolunas);

void iniciaVetor(int *vetor, int tam, int v);
void iniciaVetorDouble(double *vetor, int tam, double v);
void liberaMatriz(int **matriz, int nlinhas);

int carregaParN(char nomearq[]);
void carregaMatriz(char nomearq[], int **matriz);
void carregaVetor(char nomearq[], int *vetor);

void imprimRel(
    double fostar,
    const char *nameINST,
    const char *nameFORM,
    double gap,
    double nodect,
    int g,
    int status,
    double runtime
);

#endif