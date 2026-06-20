import os
import json
import pandas as pd
import joblib
import optuna

from sklearn.model_selection import train_test_split, StratifiedKFold, cross_validate
from sklearn.metrics import (
    accuracy_score,
    precision_score,
    recall_score,
    f1_score,
    balanced_accuracy_score,
    roc_auc_score,
    confusion_matrix
)

from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import (
    RandomForestClassifier,
    ExtraTreesClassifier,
    GradientBoostingClassifier,
    HistGradientBoostingClassifier,
    AdaBoostClassifier,
    BaggingClassifier
)

from xgboost import XGBClassifier


DATASET = "data/ia_dataset_training.csv"
RANDOM_STATE = 123
N_SPLITS = 5
N_TRIALS_OPTUNA = 100
TOP_K_OPTUNA = 2

FEATURES = ["g", "num_cities", "prize", "penalty", "avg_distance", "pa"]

os.makedirs("data", exist_ok=True)
os.makedirs("joblib", exist_ok=True)


def get_models():
    return {
        "logistic_regression": LogisticRegression(
            max_iter=2000,
            class_weight="balanced",
            random_state=RANDOM_STATE
        ),

        "decision_tree": DecisionTreeClassifier(
            random_state=RANDOM_STATE,
            class_weight="balanced"
        ),

        "random_forest": RandomForestClassifier(
            n_estimators=300,
            random_state=RANDOM_STATE,
            n_jobs=-1,
            class_weight="balanced"
        ),

        "extra_trees": ExtraTreesClassifier(
            n_estimators=300,
            random_state=RANDOM_STATE,
            n_jobs=-1,
            class_weight="balanced"
        ),

        "gradient_boosting": GradientBoostingClassifier(
            n_estimators=300,
            learning_rate=0.05,
            random_state=RANDOM_STATE
        ),

        "hist_gradient_boosting": HistGradientBoostingClassifier(
            max_iter=300,
            learning_rate=0.05,
            random_state=RANDOM_STATE
        ),

        "adaboost": AdaBoostClassifier(
            n_estimators=300,
            learning_rate=0.05,
            random_state=RANDOM_STATE
        ),

        "bagging": BaggingClassifier(
            n_estimators=300,
            random_state=RANDOM_STATE,
            n_jobs=-1
        ),

        "xgboost": XGBClassifier(
            n_estimators=300,
            max_depth=6,
            learning_rate=0.05,
            subsample=0.8,
            colsample_bytree=0.8,
            objective="binary:logistic",
            eval_metric="logloss",
            random_state=RANDOM_STATE,
            n_jobs=-1
        )
    }


def evaluate_8020(X, y, models):
    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,
        random_state=RANDOM_STATE,
        stratify=y
    )

    results = []

    for name, model in models.items():
        print(f"\nTreinando 80/20: {name}")

        model.fit(X_train, y_train)

        y_pred = model.predict(X_test)
        y_score = model.predict_proba(X_test)[:, 1]

        tn, fp, fn, tp = confusion_matrix(y_test, y_pred).ravel()

        results.append({
            "model": name,
            "accuracy": accuracy_score(y_test, y_pred),
            "balanced_accuracy": balanced_accuracy_score(y_test, y_pred),
            "precision": precision_score(y_test, y_pred, zero_division=0),
            "recall": recall_score(y_test, y_pred, zero_division=0),
            "f1_score": f1_score(y_test, y_pred, zero_division=0),
            "roc_auc": roc_auc_score(y_test, y_score),
            "tn": tn,
            "fp": fp,
            "fn": fn,
            "tp": tp
        })

    results_df = pd.DataFrame(results).sort_values(
        by=["roc_auc", "f1_score", "balanced_accuracy"],
        ascending=False
    )

    results_df.to_csv(
        "data/model_comparison_80_20_results.csv",
        sep=";",
        index=False
    )

    return results_df


def evaluate_cross_validation(X, y, models):
    cv = StratifiedKFold(
        n_splits=N_SPLITS,
        shuffle=True,
        random_state=RANDOM_STATE
    )

    scoring = {
        "accuracy": "accuracy",
        "balanced_accuracy": "balanced_accuracy",
        "precision": "precision",
        "recall": "recall",
        "f1": "f1",
        "roc_auc": "roc_auc"
    }

    results = []

    for name, model in models.items():
        print(f"\nValidação cruzada estratificada: {name}")

        scores = cross_validate(
            model,
            X,
            y,
            cv=cv,
            scoring=scoring,
            n_jobs=-1,
            return_train_score=False
        )

        row = {"model": name}

        for metric in scoring.keys():
            values = scores[f"test_{metric}"]
            row[f"{metric}_mean"] = values.mean()
            row[f"{metric}_std"] = values.std()

        results.append(row)

    results_df = pd.DataFrame(results).sort_values(
        by=["roc_auc_mean", "f1_mean", "balanced_accuracy_mean"],
        ascending=False
    )

    results_df.to_csv(
        "data/model_comparison_cross_validation_results.csv",
        sep=";",
        index=False
    )

    return results_df


def create_model_from_params(model_name, params):
    if model_name == "hist_gradient_boosting":
        return HistGradientBoostingClassifier(
            **params,
            random_state=RANDOM_STATE
        )

    if model_name == "xgboost":
        return XGBClassifier(
            **params,
            objective="binary:logistic",
            eval_metric="logloss",
            random_state=RANDOM_STATE,
            n_jobs=-1
        )

    raise ValueError(f"Optuna não configurado para o modelo: {model_name}")


def suggest_params(trial, model_name):
    if model_name == "hist_gradient_boosting":
        return {
            "max_iter": trial.suggest_int("max_iter", 100, 800),
            "learning_rate": trial.suggest_float("learning_rate", 0.01, 0.2, log=True),
            "max_leaf_nodes": trial.suggest_int("max_leaf_nodes", 15, 127),
            "min_samples_leaf": trial.suggest_int("min_samples_leaf", 10, 100),
            "l2_regularization": trial.suggest_float(
                "l2_regularization",
                1e-8,
                10.0,
                log=True
            )
        }

    if model_name == "xgboost":
        return {
            "n_estimators": trial.suggest_int("n_estimators", 200, 1200),
            "max_depth": trial.suggest_int("max_depth", 3, 10),
            "learning_rate": trial.suggest_float("learning_rate", 0.01, 0.2, log=True),
            "subsample": trial.suggest_float("subsample", 0.6, 1.0),
            "colsample_bytree": trial.suggest_float("colsample_bytree", 0.6, 1.0),
            "min_child_weight": trial.suggest_int("min_child_weight", 1, 10),
            "gamma": trial.suggest_float("gamma", 0.0, 5.0),
            "reg_alpha": trial.suggest_float("reg_alpha", 1e-8, 10.0, log=True),
            "reg_lambda": trial.suggest_float("reg_lambda", 1e-8, 10.0, log=True)
        }

    raise ValueError(f"Optuna não configurado para o modelo: {model_name}")


def optimize_model(model_name, X, y):
    cv = StratifiedKFold(
        n_splits=N_SPLITS,
        shuffle=True,
        random_state=RANDOM_STATE
    )

    def objective(trial):
        params = suggest_params(trial, model_name)
        model = create_model_from_params(model_name, params)

        scores = cross_validate(
            model,
            X,
            y,
            cv=cv,
            scoring={"roc_auc": "roc_auc"},
            n_jobs=-1,
            return_train_score=False
        )

        return scores["test_roc_auc"].mean()

    print(f"\nRodando Optuna para: {model_name}")

    study = optuna.create_study(
        direction="maximize",
        study_name=f"{model_name}_optuna"
    )

    study.optimize(
        objective,
        n_trials=N_TRIALS_OPTUNA
    )

    trials_path = f"data/optuna_trials_{model_name}.csv"
    study.trials_dataframe().to_csv(
        trials_path,
        sep=";",
        index=False
    )

    result = {
        "model": model_name,
        "best_roc_auc_cv": study.best_value,
        **study.best_params
    }

    print(f"\nMelhor resultado Optuna para {model_name}:")
    print(result)

    return result, study


def train_final_model(best_result, X, y):
    model_name = best_result["model"]

    if model_name == "hist_gradient_boosting":
        params = {
            "max_iter": int(best_result["max_iter"]),
            "learning_rate": float(best_result["learning_rate"]),
            "max_leaf_nodes": int(best_result["max_leaf_nodes"]),
            "min_samples_leaf": int(best_result["min_samples_leaf"]),
            "l2_regularization": float(best_result["l2_regularization"])
        }

    elif model_name == "xgboost":
        params = {
            "n_estimators": int(best_result["n_estimators"]),
            "max_depth": int(best_result["max_depth"]),
            "learning_rate": float(best_result["learning_rate"]),
            "subsample": float(best_result["subsample"]),
            "colsample_bytree": float(best_result["colsample_bytree"]),
            "min_child_weight": int(best_result["min_child_weight"]),
            "gamma": float(best_result["gamma"]),
            "reg_alpha": float(best_result["reg_alpha"]),
            "reg_lambda": float(best_result["reg_lambda"])
        }

    else:
        raise ValueError(f"Modelo não suportado: {model_name}")

    model = create_model_from_params(model_name, params)
    model.fit(X, y)

    model_path = f"joblib/{model_name}_optuna_best_model.joblib"
    joblib.dump(model, model_path)

    with open("data/best_optuna_model_params.json", "w") as f:
        json.dump(
            {
                "model": model_name,
                "best_roc_auc_cv": float(best_result["best_roc_auc_cv"]),
                "params": params
            },
            f,
            indent=4
        )

    return model, model_path


def main():
    df = pd.read_csv(DATASET, sep=";")

    X = df[FEATURES]
    y = df["target"]

    target_counts = y.value_counts().reset_index()
    target_counts.columns = ["target", "count"]

    target_proportions = y.value_counts(normalize=True).reset_index()
    target_proportions.columns = ["target", "proportion"]

    target_distribution = target_counts.merge(target_proportions, on="target")
    target_distribution.to_csv(
        "data/target_distribution.csv",
        sep=";",
        index=False
    )

    print("\nDistribuição da variável target:")
    print(target_distribution)

    results_8020 = evaluate_8020(X, y, get_models())
    print("\nResultados 80/20:")
    print(results_8020.to_string(index=False))

    results_cv = evaluate_cross_validation(X, y, get_models())
    print("\nResultados validação cruzada estratificada:")
    print(results_cv.to_string(index=False))

    top_models = results_cv.head(TOP_K_OPTUNA)["model"].tolist()

    print(f"\nModelos selecionados para Optuna: {top_models}")

    optuna_results = []

    for model_name in top_models:
        result, study = optimize_model(model_name, X, y)
        optuna_results.append(result)

    optuna_results_df = pd.DataFrame(optuna_results).sort_values(
        by="best_roc_auc_cv",
        ascending=False
    )

    optuna_results_df.to_csv(
        "data/optuna_best_results.csv",
        sep=";",
        index=False
    )

    print("\nResultados finais do Optuna:")
    print(optuna_results_df.to_string(index=False))

    best_result = optuna_results_df.iloc[0].to_dict()

    final_model, model_path = train_final_model(best_result, X, y)

    print("\nMelhor modelo após Optuna:")
    print(f"Modelo: {best_result['model']}")
    print(f"ROC-AUC CV: {best_result['best_roc_auc_cv']:.6f}")

    print("\nArquivos gerados:")
    print("data/target_distribution.csv")
    print("data/model_comparison_80_20_results.csv")
    print("data/model_comparison_cross_validation_results.csv")
    print("data/optuna_best_results.csv")
    print("data/best_optuna_model_params.json")
    print("data/optuna_trials_hist_gradient_boosting.csv")
    print("data/optuna_trials_xgboost.csv")
    print(model_path)


if __name__ == "__main__":
    main()