import pandas as pd
import joblib

from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, roc_auc_score
from xgboost import XGBClassifier

DATASET = "ia_dataset_training.csv"

df = pd.read_csv(DATASET, sep=";")

X = df[["g", "num_cities", "prize", "penalty", "avg_distance", "pa"]]
y = df["target"]

X_train, X_test, y_train, y_test = train_test_split(
    X,
    y,
    test_size=0.2,
    random_state=123,
    stratify=y
)

models = {
    "random_forest": RandomForestClassifier(
        n_estimators=300,
        max_depth=None,
        random_state=123,
        n_jobs=-1,
        class_weight="balanced"
    ),

    "xgboost": XGBClassifier(
        n_estimators=1000,
        max_depth=8,
        learning_rate=0.03,
        subsample=0.8,
        colsample_bytree=0.8,
        objective="binary:logistic",
        eval_metric="logloss",
        random_state=123,
        n_jobs=-1
    )
}

for name, model in models.items():
    print(f"\nTreinando modelo: {name}")

    model.fit(X_train, y_train)

    print("\nImportância das features:")
    importances = model.feature_importances_

    for feature, importance in zip(X.columns, importances):
        print(f"{feature}: {importance:.6f}")

    y_pred = model.predict(X_test)
    y_score = model.predict_proba(X_test)[:, 1]

    print(y_score.min())
    print(y_score.max())
    print(y_score.mean())

    print(classification_report(y_test, y_pred))
    print(f"ROC-AUC: {roc_auc_score(y_test, y_score):.4f}")

    joblib.dump(model, f"{name}_model.joblib")
    print(f"Modelo salvo em: {name}_model.joblib")