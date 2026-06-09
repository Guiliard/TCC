#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

SUMMARY_FILE="$PROJECT_DIR/results/new_results/tcc_summary_results.tsv"
DATASET_DIR="$PROJECT_DIR/dataset"

OUT_DIR="$PROJECT_DIR/results/new_results"
DETAIL_TSV="$OUT_DIR/tcc_detailed_by_group.tsv"
DETAIL_TABLE="$OUT_DIR/tcc_detailed_by_group_table.txt"

SMALL_TSV="$OUT_DIR/tcc_detailed_small.tsv"
MEDIUM_TSV="$OUT_DIR/tcc_detailed_medium.tsv"
LARGE_TSV="$OUT_DIR/tcc_detailed_large.tsv"

python3 <<PY
import csv

summary_file = "$SUMMARY_FILE"
dataset_dir = "$DATASET_DIR"

detail_tsv = "$DETAIL_TSV"
small_tsv = "$SMALL_TSV"
medium_tsv = "$MEDIUM_TSV"
large_tsv = "$LARGE_TSV"

def read_num_cities(instance):
    n_file = f"{dataset_dir}/{instance}/{instance}_N.pctsp"

    with open(n_file, "r", encoding="utf-8") as f:
        return int(f.read().split()[0])

def get_group(n):
    if n <= 100:
        return "Pequeno porte"
    if n <= 250:
        return "Médio porte"
    return "Grande porte"

fieldnames = [
    "grupo",
    "instance",
    "num_cities",
    "g",
    "gap_gurobi_percent",
    "gap_UB_percent",
    "mean_time",
    "gurobi_time",
    "faster_than_gurobi"
]

rows = []

with open(summary_file, "r", encoding="utf-8") as f:
    reader = csv.DictReader(f, delimiter="\t")

    for row in reader:
        instance = row["instance"]
        n = read_num_cities(instance)
        group = get_group(n)

        rows.append({
            "grupo": group,
            "instance": instance,
            "num_cities": n,
            "g": row["g"],
            "gap_gurobi_percent": row["gap_gurobi_percent"],
            "gap_UB_percent": row["gap_UB_percent"],
            "mean_time": row["mean_time"],
            "gurobi_time": row["gurobi_time"],
            "faster_than_gurobi": row["faster_than_gurobi"]
        })

def write_file(path, data):
    with open(path, "w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, delimiter="\t")
        writer.writeheader()
        writer.writerows(data)

write_file(detail_tsv, rows)
write_file(small_tsv, [r for r in rows if r["grupo"] == "Pequeno porte"])
write_file(medium_tsv, [r for r in rows if r["grupo"] == "Médio porte"])
write_file(large_tsv, [r for r in rows if r["grupo"] == "Grande porte"])

print(f"Detalhamento geral salvo em: {detail_tsv}")
print(f"Pequeno porte salvo em: {small_tsv}")
print(f"Médio porte salvo em: {medium_tsv}")
print(f"Grande porte salvo em: {large_tsv}")
PY

if [ $? -ne 0 ]; then
    echo "Erro ao gerar detalhamento por grupo."
    exit 1
fi

column -t -s $'\t' "$DETAIL_TSV" > "$DETAIL_TABLE"

echo "Tabela formatada salva em: $DETAIL_TABLE"