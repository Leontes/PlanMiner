#!/usr/bin/env python3
"""Genera las trazas de prueba de tests/data/traces: los datos fijos de los tests unitarios y
del oráculo (para evaluar PlanMiner está scripts/benchmark.py).

Lee tests/data/traces/dataset.toml. Para cada problema, el plan es
<colección>/<problema>.plan: si falta (o con --replan) lo busca PyPlanner. Para cada variante,
``pddldomains trace`` escribe la traza <colección>/<problema>.<variante>.json. Las colecciones y
los problemas (``problems``) son los del catálogo del submódulo vendor/PDDLDomains; los generados
(``generated``) están guardados en <colección>/<problema>.pddl y usan el dominio del catálogo.

Las trazas se escriben siempre en un directorio temporal con el plan (y el problema y el dominio
de los generados), porque guardan las rutas tal como se dan. Con --check no escribe nada:
regenera así las trazas a partir de los planes guardados y comprueba que coinciden con las
guardadas.

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
    """Un problema de una colección del catálogo: ipc-2000/blocks-strips-untyped y p01. Si es
    generado, el problema está con sus trazas."""

    collection: str
    problem: str
    generated: bool = False

    @property
    def directory(self) -> str:
        """El directorio de sus trazas: el nombre corto de la colección."""
        return self.collection.rsplit("/", 1)[-1]

    @property
    def domain_file(self) -> Path:
        return CATALOG / self.collection / "domain.pddl"

    @property
    def problem_file(self) -> Path:
        if self.generated:
            return DATASET.parent / self.directory / f"{self.problem}.pddl"
        return CATALOG / self.collection / f"{self.problem}.pddl"


def run(command: list[str], cwd: Path) -> None:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        raise DatasetError(
            f"{' '.join(command)} ha terminado con {result.returncode}\n{result.stderr.strip()}"
        )


def solve(task: Task, planner: dict[str, Any], plan: Path) -> None:
    """Busca el plan con PyPlanner."""
    run(
        [sys.executable, "-m", "pyplanner", "solve", "-j", "1", "-c", planner["config"]]
        + ["-t", str(planner["time_limit"]), "-m", str(planner["memory_limit"]), "-o", str(plan)]
        + [str(task.domain_file), str(task.problem_file)],
        cwd=ROOT,
    )


def prepare(task: Task, saved: Path, directory: Path) -> None:
    """Copia en directory lo que lee pddldomains trace: el plan guardado y, si el problema es
    generado, el problema y el dominio."""
    shutil.copy(saved / f"{task.problem}.plan", directory)
    if task.generated:
        shutil.copy(task.problem_file, directory)
        shutil.copy(task.domain_file, directory)


def trace(task: Task, variant: dict[str, Any], planner: dict[str, Any], directory: Path) -> Path:
    """Escribe la traza de la variante en directory, preparado con prepare: la traza guarda las
    rutas tal como se dan, y así son siempre <problema>.plan (y domain.pddl y <problema>.pddl)."""
    output = f"{task.problem}.{variant['name']}.json"
    command = [sys.executable, "-m", "pddldomains", "--catalog", str(CATALOG), "trace"]
    if task.generated:
        command += ["domain.pddl", f"{task.problem}.pddl", f"{task.problem}.plan"]
    else:
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
        Task(entry["collection"], problem, generated)
        for entry in dataset["collections"]
        for key, generated in (("problems", False), ("generated", True))
        for problem in entry.get(key, [])
    ]
    for task in tasks:
        if not task.domain_file.is_file():
            raise DatasetError(f"no está en el catálogo: {task.collection}")
        if not task.problem_file.is_file():
            where = "con las trazas" if task.generated else "en el catálogo"
            raise DatasetError(f"no está {where}: {task.collection}/{task.problem}")
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
        with tempfile.TemporaryDirectory(prefix="planminer-traces-") as tmp:
            prepare(task, directory, Path(tmp))
            for variant in variants:
                shutil.copy(trace(task, variant, planner, Path(tmp)), directory)
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
            prepare(task, saved, directory)
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
