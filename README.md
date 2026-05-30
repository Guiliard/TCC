# Problema do Caixeiro Viajante com Coleta de Prêmios

## Sobre o Problema

O Problema do Caixeiro Viajante com Coleta de Prêmios (PCVCP) consiste em determinar uma rota de custo mínimo em um grafo, visitando apenas um subconjunto das cidades disponíveis, de forma que um prêmio mínimo seja coletado. Diferentemente do Problema do Caixeiro Viajante clássico (TSP), nem todas as cidades precisam ser visitadas.

Neste trabalho, são consideradas instâncias assimétricas, nas quais o custo de deslocamento entre duas cidades pode variar conforme a direção percorrida.

**Objetivo:** Minimizar o custo total da rota considerando deslocamentos, prêmios e penalidades.

---

## Algoritmo Implementado

O projeto implementa uma abordagem híbrida baseada em heurísticas e refinamento exato, utilizando:

- **GRASP** (Greedy Randomized Adaptive Search Procedure)
- **Busca Local por Vizinhança Variável (VND)**
- **Construção por Nearest Neighbor (NN)**
- **Construção por Cheapest Insertion (CI)**
- **Refinamento exato com solver Concorde**

Além disso, a abordagem utiliza paralelismo com OpenMP para execução concorrente das iterações do GRASP.

---

## Estrutura do Projeto

```text
src/
├── main.c                  # Fluxo principal e execução
├── environment/            # Inicialização do problema
├── heuristics/
│   ├── construction/       # Construção GRASP, NN e CI
│   └── local_search/       # Busca local e VND
├── calculate/              # Função objetivo e métricas
├── convert/                # Conversões simétrica/assimétrica
├── reader/                 # Leitura das instâncias
├── solver/                 # Integração com Concorde
├── metrics/                # Tempo e desempenho
└── utils/                  # Funções auxiliares

dataset/                    # Instâncias do problema
concorde/                   # Solver Concorde
qsopt/                      # Biblioteca QSopt
irace/                      # Calibração automática de parâmetros
results/                    # Resultados experimentais
gurobi/                     # Modelos e resultados do Gurobi
```

---

## Dependências

O projeto foi desenvolvido em linguagem C utilizando:

- GCC
- OpenMP
- Concorde TSP Solver
- QSopt
- irace (R)
- Gurobi (opcional)

---

## Compilação

### Compilar o Concorde com QSopt

```bash
cd concorde

make clean 2>/dev/null || true

./configure --with-qsopt="$(realpath ../qsopt)" || ./configure --with-cplex="$(realpath ../cplex)"

make

cd ..

grep "LPSOLVER_INTERFACE" concorde/LP/Makefile
```

---

### Compilar o Projeto

```bash
make clean
make LP_SOLVER=QSOPT || make LP_SOLVER=CPLEX
```

O executável será gerado em:

```text
build/executable
```

---

## Execução

Exemplo de execução:

```bash
./build/executable --instance stilt316_16 --g 2 --alpha 0.3971 --max-iter 175 --selection ordered --seed 123
```

---

## Estrutura das Instâncias

Cada instância possui arquivos referentes a:

- Número de cidades
- Prêmios
- Penalidades
- Matriz de distâncias assimétrica

Os arquivos estão organizados em:

```text
dataset/NOME_DA_INSTANCIA/ -> dataset/stilt316_16/
```

---

## Calibração com irace

Os arquivos de calibração estão disponíveis em:

```text
irace/
```

Para instalar o irace no R:

```bash
R
install.packages("irace")
q()
```

Execução:

```bash
cd irace

chmod +x target-runner

irace --scenario scenario.txt

cd ..
```

---

## Resultados

Os resultados experimentais são armazenados em:

```text
results/tcc/
```

O projeto também inclui resultados obtidos com o solver comercial Gurobi para fins de comparação.

---

## Observações

- O projeto foi desenvolvido e testado em ambiente Linux/WSL.
- O Concorde deve ser compilado antes da compilação do executável principal.
- O número de threads utilizado pelo algoritmo é definido dinamicamente em tempo de execução, utilizando os processadores lógicos disponíveis na máquina.
- O Gurobi é opcional e requer licença ativa para execução.