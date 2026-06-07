import pandas as pd
import joblib

model = joblib.load("xgboost_model.joblib")

X = pd.DataFrame({
    "g": [2],
    "num_cities": [16],
    "prize": [83],
    "penalty": [83],
    "avg_distance": [436.666687],
    "pa": [15.776335]
})

score = model.predict_proba(X)[0][1]

print(score)