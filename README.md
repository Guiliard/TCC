# TCC - Prize-Collecting TSP Solver

Implementação de um solver para o Problema do Caixeiro Viajante com Coleta de Prêmios (Prize-Collecting Traveling Salesman Problem - PCTSP) utilizando o solver Concorde para TSP simétrico.

## Descrição

O projeto resolve o PCTSP convertendo o problema assimétrico original em um TSP simétrico, resolvendo com o Concorde, e então convertendo a solução de volta para o problema original. Além disso, implementa heurísticas de busca local para melhorar a solução.

## Dependências

- **GCC** (compilador C)
- **Concorde TSP Solver** (incluído em `concorde/`)
- **QSopt** (linear programming solver, incluído em `qsopt/`)
- **pthread** e **math** libraries

## Estrutura do Projeto

```
TCC/
├── src/
│   ├── main.c                      # Programa principal
│   ├── reader/                     # Leitura de arquivos .pctsp
│   ├── environment/                # Inicialização do ambiente
│   ├── heuristics/
│   │   ├── construction/           # Construção de solução inicial
│   │   └── local_search/           # Buscas locais (insertion, swap, drop)
│   ├── convert/
│   │   ├── symmetric.c             # Conversão para TSP simétrico
│   │   ├── assymmetric.c           # Conversão de volta para assimétrico
│   │   └── min_cost.c              # Otimização de custo mínimo
│   ├── solver/                     # Interface com Concorde
│   ├── calculate/                  # Cálculo de custos e função objetivo
│   └── utils/                      # Utilidades gerais
├── dataset/                        # Instâncias de teste .pctsp
├── concorde/                       # Solver Concorde
├── qsopt/                          # Solver QSopt
└── Makefile
```

## Como Compilar

```bash
make clean    # Limpa builds anteriores
make          # Compila o projeto
```

## Como Executar

```bash
make run      # Compila e executa
```

Ou diretamente:

```bash
./build/executable
```

## Dataset

O projeto utiliza instâncias do problema PCTSP no formato `.pctsp`. As instâncias de teste estão na pasta `dataset/`:

- `atex1_C.pctsp`
- `atex1_N.pctsp`
- `atex1_P.pctsp`
- `atex1_w100.pctsp`

Para usar uma instância diferente, modifique a linha em `src/main.c`:

```c
init_environment("dataset/atex1", &all_cities, ...);
```

## Algoritmo

1. **Inicialização**: Lê o arquivo PCTSP e configura o ambiente
2. **Construção**: Gera solução inicial aleatória respeitando meta de prêmios
3. **Simetrização**: Converte problema assimétrico em TSP simétrico
4. **Resolução**: Usa Concorde para resolver TSP simétrico
5. **Conversão**: Converte solução simétrica de volta para assimétrica
6. **Otimização**: Ajusta direção do tour para minimizar custo
7. **Busca Local**: Aplica heurísticas de melhoria:
   - Random Insertion (adiciona cidade)
   - Random Swap (troca duas cidades)
   - Random Drop (remove cidade)
8. **Avaliação**: Calcula função objetivo (custo + penalidades)

## Função Objetivo

```
FO = custo_tour + penalidades_cidades_não_visitadas
```

Onde as penalidades são balanceadas pelo parâmetro `alpha` calculado automaticamente.

## Limpeza

```bash
make clean    # Remove arquivos compilados
```