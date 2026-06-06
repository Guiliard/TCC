#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

EXEC="$PROJECT_DIR/build/executable"
DATASET_DIR="$PROJECT_DIR/dataset"

BOUNDS_FILE="$PROJECT_DIR/doc/bounds.txt"
GUROBI_FILE="$PROJECT_DIR/gurobi/results/gurobi_results_formatted.txt"

OUT_DIR="$PROJECT_DIR/results/tcc"
RAW_FILE="$OUT_DIR/tcc_raw_results.tsv"
TSV_FILE="$OUT_DIR/tcc_summary_results.tsv"
TABLE_FILE="$OUT_DIR/tcc_summary_results_table.txt"
REPORT_FILE="$OUT_DIR/tcc_report.txt"

ALPHA="0.3971"
MAX_ITER="175"
SELECTION="ordered"
SEED="123"

MIN_REPETITIONS="1"
MAX_REPETITIONS="10"
TIME_LIMIT="3600"

START_INSTANCE="stilt100_8"
START_G="5"
started=0

mkdir -p "$OUT_DIR"

# Se quiser começar tudo do zero, descomente a linha abaixo.
# echo -e "instance\tg\trun\tsolution\ttime\tstatus" > "$RAW_FILE"

if [ ! -x "$EXEC" ]; then
    echo "Executável não encontrado: $EXEC"
    exit 1
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

        if [ "$instance" = "$START_INSTANCE" ] && [ "$g" -lt "$START_G" ]; then
            continue
        fi

        current_max_repetitions=$MAX_REPETITIONS

        for run in $(seq 1 "$MAX_REPETITIONS"); do
            echo "Rodando: instance=$instance g=$g run=$run"

            output=$("$EXEC" \
                --instance "$instance" \
                --g "$g" \
                --alpha "$ALPHA" \
                --max-iter "$MAX_ITER" \
                --selection "$SELECTION" \
                --seed "$SEED" 2>/dev/null)

            status=$?

            if [ $status -ne 0 ]; then
                echo -e "$instance\t$g\t$run\t\t\t$status" >> "$RAW_FILE"
                continue
            fi

            solution=$(echo "$output" | sed -n '1p')
            time=$(echo "$output" | sed -n '2p')

            echo -e "$instance\t$g\t$run\t$solution\t$time\tOK" >> "$RAW_FILE"

            if [ "$run" -eq 1 ]; then
                passou=$(awk "BEGIN {print ($time > $TIME_LIMIT)}")

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

python3 <<PY
import csv
from statistics import mean, stdev
from collections import defaultdict

raw_file = "$RAW_FILE"
summary_file = "$TSV_FILE"
report_file = "$REPORT_FILE"
bounds_file = "$BOUNDS_FILE"
gurobi_file = "$GUROBI_FILE"

def load_gurobi(path):
    data = {}
    with open(path, "r", encoding="utf-8") as f:
        next(f, None)
        for line in f:
            parts = line.split()
            if len(parts) < 5:
                continue
            instance = parts[0]
            g = int(parts[1])
            data[(instance, g)] = {
                "solution": float(parts[2]),
                "time": float(parts[3]),
                "status": parts[4]
            }
    return data

def load_bounds(path):
    data = {}
    with open(path, "r", encoding="utf-8") as f:
        next(f, None)
        for line in f:
            parts = line.split()
            if len(parts) < 5:
                continue
            instance = parts[0]
            g = int(parts[1])
            data[(instance, g)] = {
                "ub": float(parts[2])
            }
    return data

def gap(value, ref):
    if ref is None or ref == 0:
        return ""
    return 100.0 * (value - ref) / ref

gurobi = load_gurobi(gurobi_file)
bounds = load_bounds(bounds_file)

runs = defaultdict(list)

with open(raw_file, "r", encoding="utf-8") as f:
    reader = csv.DictReader(f, delimiter="\t")
    for row in reader:
        if row["status"] != "OK":
            continue
        key = (row["instance"], int(row["g"]))
        runs[key].append({
            "solution": float(row["solution"]),
            "time": float(row["time"])
        })

faster_than_gurobi = 0
compared_with_gurobi = 0

with open(summary_file, "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f, delimiter="\t")

    writer.writerow([
        "instance",
        "g",
        "runs",
        "mean_solution",
        "std_solution",
        "best_solution",
        "mean_time",
        "std_time",
        "best_time",
        "gurobi_solution",
        "gurobi_time",
        "gurobi_status",
        "gap_gurobi_percent",
        "faster_than_gurobi",
        "UB",
        "gap_UB_percent"
    ])

    for key in sorted(runs.keys()):
        instance, g = key
        values = runs[key]

        sols = [x["solution"] for x in values]
        times = [x["time"] for x in values]

        mean_sol = mean(sols)
        std_sol = stdev(sols) if len(sols) > 1 else 0.0
        best_sol = min(sols)

        mean_time = mean(times)
        std_time = stdev(times) if len(times) > 1 else 0.0
        best_time = min(times)

        gb = gurobi.get(key)
        bd = bounds.get(key)

        gurobi_solution = gb["solution"] if gb else ""
        gurobi_time = gb["time"] if gb else ""
        gurobi_status = gb["status"] if gb else ""

        ub = bd["ub"] if bd else ""

        gap_gurobi = gap(mean_sol, gurobi_solution) if gb else ""
        gap_ub = gap(mean_sol, ub) if bd else ""

        faster = ""
        if gb:
            compared_with_gurobi += 1
            faster = mean_time < gurobi_time
            if faster:
                faster_than_gurobi += 1

        writer.writerow([
            instance,
            g,
            len(values),
            f"{mean_sol:.2f}",
            f"{std_sol:.2f}",
            f"{best_sol:.2f}",
            f"{mean_time:.4f}",
            f"{std_time:.4f}",
            f"{best_time:.4f}",
            gurobi_solution,
            gurobi_time,
            gurobi_status,
            f"{gap_gurobi:.4f}" if gap_gurobi != "" else "",
            faster,
            ub,
            f"{gap_ub:.4f}" if gap_ub != "" else ""
        ])

with open(report_file, "w", encoding="utf-8") as f:
    f.write("Resumo TCC\\n")
    f.write("====================\\n")
    f.write(f"Arquivo bruto: {raw_file}\\n")
    f.write(f"Arquivo resumido TSV: {summary_file}\\n")
    f.write(f"Total instancia-g avaliados: {len(runs)}\\n")
    f.write(f"Comparações com Gurobi: {compared_with_gurobi}\\n")
    f.write(f"TCC mais rápido que Gurobi: {faster_than_gurobi}\\n")

print(f"TSV salvo em: {summary_file}")
print(f"Relatório salvo em: {report_file}")
PY

column -t -s $'\t' "$TSV_FILE" > "$TABLE_FILE"

echo
echo "TSV salvo em: $TSV_FILE"
echo "Tabela formatada salva em: $TABLE_FILE"
echo "Relatório salvo em: $REPORT_FILE"