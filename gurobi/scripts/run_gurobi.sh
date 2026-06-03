#!/usr/bin/env bash

set -euo pipefail

EXEC="./build/rscpGRB"
DATASET_DIR="../dataset"
RESULTS_DIR="results/new_results"

TSV_FILE="$RESULTS_DIR/gurobi_results.tsv"
TABLE_FILE="$RESULTS_DIR/gurobi_results_formatted.txt"
REL_FILE="$RESULTS_DIR/relatorio.rel"
MAP_FILE="$RESULTS_DIR/map_solutions.csv"

mkdir -p "$RESULTS_DIR"

echo -e "instance\tg\tsolution\tgap\ttime\tstatus" > "$TSV_FILE"
echo "instance;g;gap;cities" > "$MAP_FILE"

rm -f "$REL_FILE"

for instance_path in "$DATASET_DIR"/*; do
    if [ ! -d "$instance_path" ]; then
        continue
    fi

    instance=$(basename "$instance_path")

    for g in 2 5 8; do
        echo "Rodando instancia = $instance g = $g"

        "$EXEC" "$g" "$instance"

        last_line=$(tail -n 1 "$REL_FILE")

        echo "$last_line" | awk -F',' '{
            gsub(/^ +| +$/, "", $1);
            gsub(/^ +| +$/, "", $2);
            gsub(/^ +| +$/, "", $4);
            gsub(/^ +| +$/, "", $5);
            gsub(/^ +| +$/, "", $7);
            gsub(/^ +| +$/, "", $8);

            print $1 "\t" $2 "\t" $4 "\t" $5 "\t" $8 "\t" $7
        }' >> "$TSV_FILE"
    done
done

column -t -s $'\t' "$TSV_FILE" > "$TABLE_FILE"

echo
echo "TSV salvo em: $TSV_FILE"
echo "Tabela formatada salva em: $TABLE_FILE"
echo "Mapeamento IA salvo em: $MAP_FILE"