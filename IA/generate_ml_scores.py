import pandas as pd
import joblib

FEATURES_FILE = "data/city_features.csv"
MODEL_FILE = "joblib/xgboost_optuna_best_model.joblib"
OUT_FILE = "data/ml_scores_best.csv"

df = pd.read_csv(FEATURES_FILE, sep=";")

X = df[
    [
        "g",
        "num_cities",
        "prize",
        "penalty",
        "avg_distance",
        "pa"
    ]
]

model = joblib.load(MODEL_FILE)

df["ml_score"] = model.predict_proba(X)[:, 1]

df[
    [
        "instance",
        "g",
        "city",
        "ml_score"
    ]
].to_csv(
    OUT_FILE,
    sep=";",
    index=False
)

print(f"Modelo carregado: {MODEL_FILE}")
print(f"Quantidade de cidades: {len(df)}")
print(f"Score mínimo: {df['ml_score'].min():.6f}")
print(f"Score máximo: {df['ml_score'].max():.6f}")
print(f"Score médio: {df['ml_score'].mean():.6f}")

print(f"\nScores salvos em: {OUT_FILE}")