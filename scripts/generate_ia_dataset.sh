#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

EXEC="$PROJECT_DIR/build/executable"
DATASET_DIR="$PROJECT_DIR/dataset"
MAP_FILE="$PROJECT_DIR/gurobi/results/new_results/map_solutions.csv"

OUT_DIR="$PROJECT_DIR/IA"
FEATURES_FILE="$OUT_DIR/city_features.csv"
FINAL_FULL_FILE="$OUT_DIR/ia_dataset_full.csv"
FINAL_TRAINING_FILE="$OUT_DIR/ia_dataset_training.csv"

mkdir -p "$OUT_DIR"

echo "instance;g;num_cities;city;prize;penalty;avg_distance;pa" > "$FEATURES_FILE"

if [ ! -x "$EXEC" ]; then
    echo "Executável não encontrado: $EXEC"
    exit 1
fi

if [ ! -f "$MAP_FILE" ]; then
    echo "Arquivo map_solutions não encontrado: $MAP_FILE"
    exit 1
fi

for instance_path in $(find "$DATASET_DIR" -mindepth 1 -maxdepth 1 -type d | sort); do
    instance="$(basename "$instance_path")"

    for g in 2 5 8; do
        echo "Extraindo features: instance=$instance g=$g"

        output=$(cd "$PROJECT_DIR" && "$EXEC" \
            --instance "$instance" \
            --g "$g" \
            --export-ia-dataset 2>&1)

        status=$?

        if [ $status -ne 0 ]; then
            echo "Erro ao extrair features: instance=$instance g=$g"
            echo "$output"
            continue
        fi

        echo "$output" >> "$FEATURES_FILE"
    done
done

python3 <<PY
import csv
from pathlib import Path

features_file = Path("$FEATURES_FILE")
map_file = Path("$MAP_FILE")
final_full_file = Path("$FINAL_FULL_FILE")
final_training_file = Path("$FINAL_TRAINING_FILE")

solution_cities = {}

with map_file.open("r", encoding="utf-8") as f:
    reader = csv.DictReader(f, delimiter=";")

    for row in reader:
        instance = row["instance"]
        g = int(row["g"])

        raw_cities = row["cities"].strip()
        cities = set(int(x) for x in raw_cities.split()) if raw_cities else set()

        solution_cities[(instance, g)] = cities

with features_file.open("r", encoding="utf-8") as fin, \
     final_full_file.open("w", newline="", encoding="utf-8") as fout_full, \
     final_training_file.open("w", newline="", encoding="utf-8") as fout_training:

    reader = csv.DictReader(fin, delimiter=";")

    full_fieldnames = [
        "instance",
        "g",
        "num_cities",
        "city",
        "prize",
        "penalty",
        "avg_distance",
        "pa",
        "target"
    ]

    training_fieldnames = [
        "g",
        "num_cities",
        "prize",
        "penalty",
        "avg_distance",
        "pa",
        "target"
    ]

    full_writer = csv.DictWriter(fout_full, fieldnames=full_fieldnames, delimiter=";")
    training_writer = csv.DictWriter(fout_training, fieldnames=training_fieldnames, delimiter=";")

    full_writer.writeheader()
    training_writer.writeheader()

    for row in reader:
        instance = row["instance"]
        g = int(row["g"])
        city = int(row["city"])

        selected = solution_cities.get((instance, g), set())
        target = 1 if city in selected else 0

        full_writer.writerow({
            "instance": instance,
            "g": g,
            "num_cities": row["num_cities"],
            "city": city,
            "prize": row["prize"],
            "penalty": row["penalty"],
            "avg_distance": row["avg_distance"],
            "pa": row["pa"],
            "target": target
        })

        training_writer.writerow({
            "g": g,
            "num_cities": row["num_cities"],
            "prize": row["prize"],
            "penalty": row["penalty"],
            "avg_distance": row["avg_distance"],
            "pa": row["pa"],
            "target": target
        })

print(f"Base completa gerada em: {final_full_file}")
print(f"Base de treinamento gerada em: {final_training_file}")
PY