#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

RAW_FILE="$PROJECT_DIR/results/final_results_best/tcc_raw_results.tsv"

GUROBI_FILE="$PROJECT_DIR/gurobi/results/merged/gurobi_results.tsv"
BOUNDS_FILE="$PROJECT_DIR/doc/bounds.txt"

OUT_DIR="$PROJECT_DIR/results/final_results_best"
TSV_FILE="$OUT_DIR/tcc_summary_results.tsv"
TABLE_FILE="$OUT_DIR/tcc_summary_results_table.txt"
REPORT_FILE="$OUT_DIR/tcc_report.txt"

mkdir -p "$OUT_DIR"

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
        reader = csv.DictReader(f, delimiter="\\t")

        for row in reader:
            if not row["instance"] or not row["g"]:
                continue

            instance = row["instance"]
            g = int(row["g"])

            data[(instance, g)] = {
                "solution": float(row["solution"]),
                "time": float(row["time"]),
                "status": row["status"]
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
    reader = csv.DictReader(f, delimiter="\\t")

    for row in reader:
        if not row["solution"] or not row["time"]:
            continue

        key = (row["instance"], int(row["g"]))

        runs[key].append({
            "solution": float(row["solution"]),
            "time": float(row["time"])
        })

faster_than_gurobi = 0
compared_with_gurobi = 0

with open(summary_file, "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f, delimiter="\\t")

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

        gap_gurobi = gap(best_sol, gurobi_solution) if gb else ""
        gap_ub = gap(best_sol, ub) if bd else ""

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
    f.write(f"Tabela formatada: $TABLE_FILE\\n")
    f.write(f"Total instancia-g avaliados: {len(runs)}\\n")
    f.write(f"Comparações com Gurobi: {compared_with_gurobi}\\n")
    f.write(f"TCC mais rápido que Gurobi: {faster_than_gurobi}\\n")

print(f"TSV salvo em: {summary_file}")
print(f"Relatório salvo em: {report_file}")
PY

if [ $? -ne 0 ]; then
    echo "Erro ao gerar o resumo."
    exit 1
fi

column -t -s $'\t' "$TSV_FILE" > "$TABLE_FILE"

echo
echo "TSV salvo em: $TSV_FILE"
echo "Tabela formatada salva em: $TABLE_FILE"
echo "Relatório salvo em: $REPORT_FILE"