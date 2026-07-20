#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

EXEC="$PROJECT_DIR/build/executable"
DATASET_DIR="$PROJECT_DIR/dataset"

OUT_DIR="$PROJECT_DIR/results/concorde_improvement"
RAW_FILE="$OUT_DIR/concorde_breakdown.tsv"
NO_IMPROVEMENT_FILE="$OUT_DIR/concorde_no_improvement.tsv"

ALPHA="0.2191"
MAX_ITER="175"
SELECTION="ordered"
SEED="123"

concorde_calls=0

mkdir -p "$OUT_DIR"

if [ ! -f "$RAW_FILE" ]; then
    echo -e "instance\tg\tfo_construcao\tfo_busca_local\tfo_concorde\ttempo_construcao\ttempo_busca_local\ttempo_concorde" > "$RAW_FILE"
fi

if [ ! -f "$NO_IMPROVEMENT_FILE" ]; then
    echo -e "instance\tg\tfo_construcao\tfo_busca_local\tfo_concorde\ttempo_construcao\ttempo_busca_local\ttempo_concorde" > "$NO_IMPROVEMENT_FILE"
fi

if [ ! -x "$EXEC" ]; then
    echo "Executável não encontrado: $EXEC"
    exit 1
fi

for instance_path in $(find "$DATASET_DIR" -mindepth 1 -maxdepth 1 -type d | sort); do
    [ -d "$instance_path" ] || continue
    instance="$(basename "$instance_path")"

    n_file="$instance_path/${instance}_N.pctsp"
    p_file="$instance_path/${instance}_P.pctsp"
    w_file="$instance_path/${instance}_w100.pctsp"
    c_file="$instance_path/${instance}_C.pctsp"

    if [ ! -f "$n_file" ] || [ ! -f "$p_file" ] || [ ! -f "$w_file" ] || [ ! -f "$c_file" ]; then
        echo "Pulando $instance: arquivos incompletos"
        continue
    fi

    for g in 2 5 8; do
        echo "Rodando: instance=$instance g=$g"

        output=$(cd "$PROJECT_DIR" && "$EXEC" \
            --instance "$instance" \
            --g "$g" \
            --alpha "$ALPHA" \
            --max-iter "$MAX_ITER" \
            --selection "$SELECTION" \
            --seed "$SEED" 2>&1)

        status=$?

        if [ $status -ne 0 ]; then
            echo "Erro na execução: instance=$instance g=$g status=$status"
            echo "$output"
            echo "----------------------------------------"
            continue
        fi

        fo_construcao=$(echo "$output"   | awk -F'\t' '{print $1}')
        fo_busca_local=$(echo "$output"  | awk -F'\t' '{print $2}')
        fo_concorde=$(echo "$output"     | awk -F'\t' '{print $3}')
        tempo_construcao=$(echo "$output"  | awk -F'\t' '{print $4}')
        tempo_busca_local=$(echo "$output" | awk -F'\t' '{print $5}')
        tempo_concorde=$(echo "$output"    | awk -F'\t' '{print $6}')
        concorde_melhorou=$(echo "$output" | awk -F'\t' '{print $7}')

        if [ "$concorde_melhorou" = "1" ]; then
            concorde_calls=$((concorde_calls + 1))
            echo -e "$instance\t$g\t$fo_construcao\t$fo_busca_local\t$fo_concorde\t$tempo_construcao\t$tempo_busca_local\t$tempo_concorde" >> "$RAW_FILE"
            echo "Melhora #$concorde_calls encontrada: instance=$instance g=$g"
        else
            echo -e "$instance\t$g\t$fo_construcao\t$fo_busca_local\t$fo_busca_local\t$tempo_construcao\t$tempo_busca_local\t$tempo_concorde" >> "$NO_IMPROVEMENT_FILE"
        fi
    done
done

echo "Total de vezes em que o Concorde melhorou a solução: $concorde_calls"