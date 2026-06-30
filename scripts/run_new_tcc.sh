#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

EXEC="$PROJECT_DIR/build/executable"
DATASET_DIR="$PROJECT_DIR/dataset"

OUT_DIR="$PROJECT_DIR/results/final_results_best"
RAW_FILE="$OUT_DIR/tcc_raw_results.tsv"

ALPHA="0.2191"
MAX_ITER="175"
SELECTION="ordered"
SEED="123"

MIN_REPETITIONS="3"
MAX_REPETITIONS="10"
TIME_LIMIT="3600"

START_INSTANCE=""
START_G=""

mkdir -p "$OUT_DIR"

if [ ! -f "$RAW_FILE" ]; then
    echo -e "instance\tg\trun\tsolution\ttime" > "$RAW_FILE"
fi

if [ ! -x "$EXEC" ]; then
    echo "Executável não encontrado: $EXEC"
    exit 1
fi

started=1

if [ -n "$START_INSTANCE" ]; then
    started=0
fi

for instance_path in $(find "$DATASET_DIR" -mindepth 1 -maxdepth 1 -type d | sort); do
    [ -d "$instance_path" ] || continue

    instance="$(basename "$instance_path")"

    if [ "$started" -eq 0 ]; then
        if [ "$instance" = "$START_INSTANCE" ]; then
            started=1
        else
            continue
        fi
    fi

    n_file="$instance_path/${instance}_N.pctsp"
    p_file="$instance_path/${instance}_P.pctsp"
    w_file="$instance_path/${instance}_w100.pctsp"
    c_file="$instance_path/${instance}_C.pctsp"

    if [ ! -f "$n_file" ] || [ ! -f "$p_file" ] || [ ! -f "$w_file" ] || [ ! -f "$c_file" ]; then
        echo "Pulando $instance: arquivos incompletos"
        continue
    fi

    for g in 2 5 8; do
        if [ -n "$START_INSTANCE" ] && [ "$instance" = "$START_INSTANCE" ] && [ -n "$START_G" ] && [ "$g" -lt "$START_G" ]; then
            continue
        fi

        current_max_repetitions=$MAX_REPETITIONS

        for run in $(seq 1 "$MAX_REPETITIONS"); do
            echo "Rodando: instance=$instance g=$g run=$run"

            output=$(cd "$PROJECT_DIR" && "$EXEC" \
                --instance "$instance" \
                --g "$g" \
                --alpha "$ALPHA" \
                --max-iter "$MAX_ITER" \
                --selection "$SELECTION" \
                --seed "$SEED" 2>&1)

            status=$?

            if [ $status -ne 0 ]; then
                echo "Erro na execução: instance=$instance g=$g run=$run status=$status"
                echo "Saída do executável:"
                echo "$output"
                echo "----------------------------------------"
                continue
            fi

            solution=$(echo "$output" | sed -n '1p')
            time=$(echo "$output" | sed -n '2p')

            echo -e "$instance\t$g\t$run\t$solution\t$time" >> "$RAW_FILE"

            if [ "$run" -eq 1 ]; then
                passou=$(awk "BEGIN {print ($time > $TIME_LIMIT) ? 1 : 0}")

                if [ "$passou" -eq 1 ]; then
                    current_max_repetitions=$MIN_REPETITIONS

                    echo "Primeira execução demorou $time s."
                    echo "Limitando para $MIN_REPETITIONS execuções."
                fi
            fi

            if [ "$run" -ge "$current_max_repetitions" ]; then
                break
            fi
        done
    done
done