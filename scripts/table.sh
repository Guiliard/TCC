#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

OUT_DIR="$PROJECT_DIR/results/final_results"

SMALL_TSV="$OUT_DIR/tcc_detailed_small.tsv"
MEDIUM_TSV="$OUT_DIR/tcc_detailed_medium.tsv"
LARGE_TSV="$OUT_DIR/tcc_detailed_large.tsv"

SUMMARY_TSV="$OUT_DIR/tcc_group_summary.tsv"
SUMMARY_TABLE="$OUT_DIR/tcc_group_summary_table.txt"

python3 <<PY
import csv

small_tsv = "$SMALL_TSV"
medium_tsv = "$MEDIUM_TSV"
large_tsv = "$LARGE_TSV"
summary_tsv = "$SUMMARY_TSV"


def load_rows(path):
    with open(path, "r", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def summarize(group_name, rows):
    return {
        "grupo": group_name,
        "instancias": len(set(r["instance"] for r in rows)),
        "experimentos": len(rows),

        "gap_medio_gurobi_percent":
            f'{sum(float(r["gap_gurobi_percent"]) for r in rows) / len(rows):.4f}',

        "gap_medio_UB_percent":
            f'{sum(float(r["gap_UB_percent"]) for r in rows) / len(rows):.4f}',

        "tempo_medio_algoritmo":
            f'{sum(float(r["mean_time"]) for r in rows) / len(rows):.4f}',

        "tempo_medio_gurobi":
            f'{sum(float(r["gurobi_time"]) for r in rows) / len(rows):.4f}',

        "gurobi_time_limit":
            sum(float(r["gurobi_time"]) >= 3600 for r in rows),

        "algoritmo_mais_rapido":
            sum(r["faster_than_gurobi"].strip().lower() == "true"
                for r in rows),

        "melhor_que_UB":
            sum(float(r["gap_UB_percent"]) < 0 for r in rows),

        "otimos_globais":
            sum(abs(float(r["gap_gurobi_percent"])) < 1e-6
                for r in rows),

        "gap_gurobi_acima_10":
            sum(float(r["gap_gurobi_percent"]) > 10
                for r in rows)
    }


small_rows = load_rows(small_tsv)
medium_rows = load_rows(medium_tsv)
large_rows = load_rows(large_tsv)

all_rows = small_rows + medium_rows + large_rows

results = [
    summarize("Pequeno porte", small_rows),
    summarize("Médio porte", medium_rows),
    summarize("Grande porte", large_rows),
    summarize("Geral", all_rows)
]

fieldnames = list(results[0].keys())

with open(summary_tsv, "w", encoding="utf-8", newline="") as f:
    writer = csv.DictWriter(
        f,
        fieldnames=fieldnames,
        delimiter="\t"
    )

    writer.writeheader()
    writer.writerows(results)

print(f"Resumo salvo em: {summary_tsv}")
PY

if [ $? -ne 0 ]; then
    echo "Erro ao gerar resumo por grupo."
    exit 1
fi

column -t -s $'\t' "$SUMMARY_TSV" > "$SUMMARY_TABLE"

echo "Tabela formatada salva em: $SUMMARY_TABLE"