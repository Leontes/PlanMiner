#!/usr/bin/env python3
"""Genera las trazas de prueba de tests/data/traces: los datos fijos de los tests unitarios y
del oráculo (para evaluar PlanMiner está scripts/benchmark.py).

Lee tests/data/traces/dataset.toml. Para cada problema, el plan es
<colección>/<problema>.plan: si falta (o con --replan) lo busca PyPlanner. Para cada variante,
``pddldomains trace`` escribe la traza <colección>/<problema>.<variante>.json. Las colecciones y
los problemas son los del catálogo del submódulo vendor/PDDLDomains.

Con --check no escribe nada: regenera las trazas a partir de los planes guardados en un
directorio temporal y comprueba que coinciden con las guardadas.

Se ejecuta con el Python de .venv (scripts/setup_python.sh), que tiene PyPlanner y pddldomains.

Códigos de salida: 0 si todo va bien; 1 si --check encuentra diferencias; 2 si los argumentos
o el dataset no son válidos, o falla PyPlanner o pddldomains.
"""

import argparse
import filecmp
import shutil
import subprocess
import sys
import tempfile
import tomllib
from collections.abc import Sequence
from dataclasses import dataclass
from importlib.metadata import version
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parent.parent
DATASET = ROOT / "tests" / "data" / "traces" / "dataset.toml"
CATALOG = ROOT / "vendor" / "PDDLDomains" / "dominios"

# Las opciones de las variantes y su opción de pddldomains trace.
OPTIONS = {
    "world": "--world",
    "universe": "--universe",
    "missing": "--missing",
    "missing_states": "--missing-states",
    "noise": "--noise",
    "numeric_noise": "--numeric-noise",
    "seed": "--seed",
}


class DatasetError(Exception):
    pass


@dataclass(frozen=True)
class Task:
    """Un problema del catálogo: ipc-2000/blocks-strips-untyped y p01."""

    collection: str
    problem: str

    @property
    def directory(self) -> str:
        """El directorio de sus trazas: el nombre corto de la colección."""
        return self.collection.rsplit("/", 1)[-1]


def run(command: list[str], cwd: Path) -> None:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        raise DatasetError(
            f"{' '.join(command)} ha terminado con {result.returncode}\n{result.stderr.strip()}"
        )


def solve(task: Task, planner: dict[str, Any], plan: Path) -> None:
    """Busca el plan con PyPlanner."""
    files = CATALOG / task.collection
    run(
        [sys.executable, "-m", "pyplanner", "solve", "-j", "1", "-c", planner["config"]]
        + ["-t", str(planner["time_limit"]), "-m", str(planner["memory_limit"]), "-o", str(plan)]
        + [str(files / "domain.pddl"), str(files / f"{task.problem}.pddl")],
        cwd=ROOT,
    )


def trace(task: Task, variant: dict[str, Any], planner: dict[str, Any], directory: Path) -> Path:
    """Escribe la traza de la variante en directory, donde tiene que estar el plan: la traza
    guarda la ruta del plan tal como se da, y así es siempre <problema>.plan."""
    output = f"{task.problem}.{variant['name']}.json"
    command = [sys.executable, "-m", "pddldomains", "--catalog", str(CATALOG), "trace"]
    command += ["--task", f"{task.collection}/{task.problem}", f"{task.problem}.plan"]
    command += ["-o", output, "--planner", f"pyplanner {version('pyplanner')}"]
    command += ["--planner-config", planner["config"]]
    for key, value in variant.items():
        if key != "name":
            command += [OPTIONS[key], str(value)]
    run(command, cwd=directory)
    return directory / output


def load() -> tuple[dict[str, Any], list[Task], list[dict[str, Any]]]:
    """El planificador, los problemas y las variantes del dataset."""
    if not CATALOG.is_dir():
        raise DatasetError(
            f"falta el catálogo en {CATALOG}: git submodule update --init vendor/PDDLDomains"
        )
    dataset = tomllib.loads(DATASET.read_text(encoding="utf-8"))
    tasks = [
        Task(entry["collection"], problem)
        for entry in dataset["collections"]
        for problem in entry["problems"]
    ]
    for task in tasks:
        if not (CATALOG / task.collection / f"{task.problem}.pddl").is_file():
            raise DatasetError(f"no está en el catálogo: {task.collection}/{task.problem}")
    variants = dataset["variants"]
    for variant in variants:
        unknown = set(variant) - set(OPTIONS) - {"name"}
        if unknown:
            raise DatasetError(f"variante {variant['name']}: opciones desconocidas {unknown}")
    return dataset["planner"], tasks, variants


def generate(replan: bool) -> int:
    planner, tasks, variants = load()
    for task in tasks:
        directory = DATASET.parent / task.directory
        directory.mkdir(exist_ok=True)
        plan = directory / f"{task.problem}.plan"
        if replan or not plan.exists():
            solve(task, planner, plan)
        for variant in variants:
            trace(task, variant, planner, directory)
        print(f"{task.collection}/{task.problem}: {len(variants)} trazas")
    return 0


def check() -> int:
    planner, tasks, variants = load()
    differences = []
    with tempfile.TemporaryDirectory(prefix="planminer-traces-") as tmp:
        for task in tasks:
            saved = DATASET.parent / task.directory
            directory = Path(tmp) / task.directory
            directory.mkdir(exist_ok=True)
            shutil.copy(saved / f"{task.problem}.plan", directory)
            for variant in variants:
                generated = trace(task, variant, planner, directory)
                if not filecmp.cmp(generated, saved / generated.name, shallow=False):
                    differences.append(f"{task.directory}/{generated.name}")
    for difference in differences:
        print(f"distinta: {difference}")
    total = len(tasks) * len(variants)
    print(f"{total - len(differences)}/{total} trazas iguales")
    return 1 if differences else 0


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Genera las trazas de tests/data/traces.")
    group = parser.add_mutually_exclusive_group()
    group.add_argument(
        "--replan", action="store_true", help="vuelve a buscar los planes aunque estén guardados"
    )
    group.add_argument(
        "--check",
        action="store_true",
        help="comprueba que las trazas guardadas se regeneran igual, sin escribir nada",
    )
    args = parser.parse_args(argv)
    try:
        return check() if args.check else generate(args.replan)
    except (DatasetError, FileNotFoundError, KeyError, tomllib.TOMLDecodeError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
