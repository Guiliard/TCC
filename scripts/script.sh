#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

INPUT_FILE="$PROJECT_DIR/results/new_results/tcc_detailed_large.tsv"
OUT_FILE="$PROJECT_DIR/results/new_results/tabela_grande_porte_latex.txt"

python3 <<PY
import csv

input_file = "$INPUT_FILE"
out_file = "$OUT_FILE"

def esc(s):
    return s.replace("_", "\\_")

rows = []

with open(input_file, "r", encoding="utf-8") as f:
    reader = csv.DictReader(f, delimiter="\t")

    for row in reader:
        rows.append(row)

rows.sort(key=lambda r: (int(r["num_cities"]), r["instance"], int(r["g"])))

with open(out_file, "w", encoding="utf-8") as f:
    f.write(r"""\begin{table}[!htb]
\centering
\caption{Resultados completos para instâncias de grande porte.}
\label{tab:ap-gra-resultados}

\scriptsize
\setlength{\tabcolsep}{3pt}
\renewcommand{\arraystretch}{0.95}

\begin{tabular*}{\textwidth}{@{\extracolsep{\fill}} l c c c c c c}
\hline
\textbf{Instância} & \(\mathbf{n}\) & \(\mathbf{g}\) & \textbf{Gap Gurobi (\%)} & \textbf{Gap UB (\%)} & \textbf{Tempo Alg. (s)} & \textbf{Tempo Gurobi (s)} \\
\hline
""")

    last_instance = None

    for row in rows:
        instance = row["instance"]

        if last_instance is not None and instance != last_instance:
            f.write("\\hline\n")

        f.write(
            f"{esc(instance)} & "
            f"{row['num_cities']} & "
            f"{row['g']} & "
            f"{float(row['gap_gurobi_percent']):.2f} & "
            f"{float(row['gap_UB_percent']):.2f} & "
            f"{float(row['mean_time']):.2f} & "
            f"{float(row['gurobi_time']):.2f} \\\\\n"
        )

        last_instance = instance

    f.write(r"""\hline
\end{tabular*}

\renewcommand{\arraystretch}{1}
\end{table}
""")

print(f"Tabela LaTeX salva em: {out_file}")
PY