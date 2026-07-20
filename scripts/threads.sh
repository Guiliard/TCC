#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

EXEC="$PROJECT_DIR/build/executable"
OUT_DIR="$PROJECT_DIR/results/threads_scalability"
RAW_FILE="$OUT_DIR/threads_scalability.tsv"

ALPHA="0.2191"
MAX_ITER="50"
SELECTION="ordered"
SEED="123"

INSTANCES=(
    "atex5"
    "stilt100_0"
    "dc134"
    "ftv100"
    "crane316_13"
    "crane316_14"
)

THREAD_COUNTS=(1 5 10 15)
G_LEVELS=(2 5 8)

mkdir -p "$OUT_DIR"

if [ ! -f "$RAW_FILE" ]; then
    echo -e "instance\tg\tthreads\tfo\ttempo" > "$RAW_FILE"
fi

if [ ! -x "$EXEC" ]; then
    echo "Executável não encontrado: $EXEC"
    exit 1
fi

for instance in "${INSTANCES[@]}"; do
    for g in "${G_LEVELS[@]}"; do
        for threads in "${THREAD_COUNTS[@]}"; do
            echo "Rodando: instance=$instance g=$g threads=$threads"

            output=$(cd "$PROJECT_DIR" && "$EXEC" \
                --instance "$instance" \
                --g "$g" \
                --alpha "$ALPHA" \
                --max-iter "$MAX_ITER" \
                --selection "$SELECTION" \
                --seed "$SEED" \
                --threads "$threads" 2>&1)

            status=$?

            if [ $status -ne 0 ]; then
                echo "Erro na execução: instance=$instance g=$g threads=$threads status=$status"
                echo "$output"
                echo "----------------------------------------"
                continue
            fi

            fo=$(echo "$output" | sed -n '1p')
            tempo=$(echo "$output" | sed -n '2p')

            echo -e "$instance\t$g\t$threads\t$fo\t$tempo" >> "$RAW_FILE"
        done
    done
done

echo "Concluído. Resultados em $RAW_FILE"