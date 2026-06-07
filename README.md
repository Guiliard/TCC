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
- CPLEX
- irace (R)
- Gurobi (opcional)

---

## Compilação

### Instalar CPLEX

```bash

mkdir -p cplex

# Headers do CPLEX
ln -sf /opt/ibm/ILOG/CPLEX_Studio222/cplex/include/ilcplex/*.h cplex/

# Biblioteca estática
ln -sf /opt/ibm/ILOG/CPLEX_Studio222/cplex/lib/x86-64_linux/static_pic/libcplex.a cplex/libcplex.a

```

### Compilar o Concorde com QSopt ou CPLEX

```bash
cd concorde

make clean 2>/dev/null || true

chmod +x configure

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
sudo apt update

sudo apt install r-base r-base-dev libuv1-dev

R

install.packages("fs", repos="https://cloud.r-project.org")

install.packages("irace", repos="https://cloud.r-project.org")

library(irace)

irace_cmdline("--version")

q()
```

Execução:

```bash
cd irace

mkdir -p build

chmod +x target-runner

Rscript -e 'library(irace); irace::irace_cmdline("--scenario scenario.txt")'

cd ..
```

---

## Aprendizado Supervisionado

```bash
cd IA

python3 -m venv venv

source venv/bin/activate

pip install -r requirements.txt
```

Para treinar os modelos:
```bash
python3 models.py
```

Para gerar o CSV que se comunica com o algoritmo:
```bash
python3 modelsCsv.py
```

Para sair do ambiente virtual:
```bash
deactivate
```

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