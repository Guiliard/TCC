import pandas as pd
import joblib

FEATURES_FILE = "city_features.csv"
MODEL_FILE = "xgboost_model.joblib"
OUT_FILE = "ml_scores.csv"

df = pd.read_csv(FEATURES_FILE, sep=";")

X = df[["g", "num_cities", "prize", "penalty", "avg_distance", "pa"]]

model = joblib.load(MODEL_FILE)

df["ml_score"] = model.predict_proba(X)[:, 1]

df[["instance", "g", "city", "ml_score"]].to_csv(
    OUT_FILE,
    sep=";",
    index=False
)

print(f"Scores salvos em: {OUT_FILE}")