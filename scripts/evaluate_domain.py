#!/usr/bin/env python3
"""Evalúa un dominio aprendido planificando con él.

Para cada problema, PyPlanner busca un plan con el dominio aprendido y VAL lo valida con el
dominio de referencia. El dominio pasa si con todos los problemas se encuentra un plan y es
válido.

Uso::

    evaluate_domain.py --learned aprendido.pddl --reference referencia.pddl \\
        --validate tools/bin/Validate p01.pddl p02.pddl ...

PyPlanner se ejecuta con el mismo intérprete que este script (``python -m pyplanner``) y con
un límite de memoria (``--memory-limit``): sin él se reserva el 80 % de la disponible, y varios
tests en paralelo agotarían la de la máquina, que en WSL congela todo. El límite de tiempo
(``--time-limit``) es el de la búsqueda; el de PyPlanner no cubre el grounding, que con un
dominio aprendido con pocas precondiciones puede no terminar, y por eso la ejecución entera se
corta al doble.

Códigos de salida: 0 si todos los planes son válidos, 1 si alguno falta o no es válido y 2
si los argumentos no son válidos.
"""

import argparse
import subprocess
import sys
import tempfile
from collections.abc import Sequence
from dataclasses import dataclass
from enum import StrEnum
from pathlib import Path

# Códigos de salida de `pyplanner solve`.
PLAN_FOUND = 0
INVALID_INPUT = 2

# La ejecución entera de PyPlanner se corta a este múltiplo del límite de tiempo: el suyo es el
# de la búsqueda y no cubre el grounding.
WALL_FACTOR = 2


class Status(StrEnum):
    """Lo que ha pasado con un problema."""

    VALID = "válido"
    INVALID = "inválido"
    NO_PLAN = "sin plan"
    ERROR = "error"


@dataclass(frozen=True)
class Outcome:
    problem: Path
    status: Status
    detail: str = ""


def _summary_line(text: str, *markers: str) -> str:
    """La primera línea que contiene alguna de las marcas o, si no hay, la última."""
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    for line in lines:
        if any(marker in line for marker in markers):
            return line
    return lines[-1] if lines else ""


@dataclass(frozen=True)
class Limits:
    time: int  # segundos
    memory: int  # MB
    #: La configuración de búsqueda de PyPlanner; sin ella, la que elige para un núcleo.
    config: str | None = None


def evaluate_problem(
    learned: Path, reference: Path, problem: Path, validate: Path, limits: Limits, workdir: Path
) -> Outcome:
    """Busca un plan con el dominio aprendido y lo valida con el de referencia."""
    plan = workdir / f"{problem.stem}.plan"
    config = ["-c", limits.config] if limits.config else []
    try:
        solve = subprocess.run(
            [sys.executable, "-m", "pyplanner", "solve", "-j", "1", *config]
            + ["-t", str(limits.time), "-m", str(limits.memory), "-o", str(plan)]
            + [str(learned), str(problem)],
            capture_output=True,
            text=True,
            check=False,
            timeout=WALL_FACTOR * limits.time,
        )
    except subprocess.TimeoutExpired:
        detail = f"Resultado: limit_reached (sin terminar en {WALL_FACTOR * limits.time} s)"
        return Outcome(problem, Status.NO_PLAN, detail)
    if solve.returncode == INVALID_INPUT:
        return Outcome(problem, Status.ERROR, _summary_line(solve.stderr, "error:"))
    if solve.returncode != PLAN_FOUND:
        return Outcome(problem, Status.NO_PLAN, _summary_line(solve.stderr, "Resultado:"))

    check = subprocess.run(
        [str(validate), str(reference), str(problem), str(plan)],
        capture_output=True,
        text=True,
        check=False,
    )
    # El resultado se decide por el texto "Plan valid", como en los tests de PyPlanner: VAL
    # termina con 0 también si el plan usa una acción que no está en el dominio de referencia
    # ("Bad plan description!").
    output = check.stdout + check.stderr
    if "Plan valid" not in output:
        detail = _summary_line(output, "Bad plan", "failed", "Goal not satisfied", "rror")
        return Outcome(problem, Status.INVALID, detail)
    steps = sum(1 for line in plan.read_text().splitlines() if line.startswith("("))
    return Outcome(problem, Status.VALID, f"{steps} acciones")


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Valida un dominio aprendido: planifica con él y valida los planes con VAL."
    )
    parser.add_argument("--learned", type=Path, required=True, help="dominio aprendido")
    parser.add_argument("--reference", type=Path, required=True, help="dominio de referencia")
    parser.add_argument("--validate", type=Path, required=True, help="ejecutable Validate de VAL")
    parser.add_argument(
        "--time-limit",
        type=int,
        default=60,
        help="segundos de búsqueda por problema (por defecto, 60; la ejecución se corta al doble)",
    )
    parser.add_argument(
        "--memory-limit",
        type=int,
        default=1024,
        help="MB de memoria de PyPlanner en cada problema (por defecto, 1024)",
    )
    parser.add_argument("problems", type=Path, nargs="+", help="problemas PDDL")
    args = parser.parse_args(argv)

    for path in [args.learned, args.reference, args.validate, *args.problems]:
        if not path.is_file():
            parser.error(f"no existe el fichero {path}")
    return args


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)

    with tempfile.TemporaryDirectory(prefix="planminer-plans-") as tmp:
        outcomes = [
            evaluate_problem(
                args.learned,
                args.reference,
                problem,
                args.validate,
                Limits(args.time_limit, args.memory_limit),
                Path(tmp),
            )
            for problem in args.problems
        ]

    for outcome in outcomes:
        print(f"{outcome.status:<9} {outcome.problem.name}: {outcome.detail}")
    valid = sum(outcome.status is Status.VALID for outcome in outcomes)
    print(f"{valid}/{len(outcomes)} planes válidos con {args.learned.name}")
    return 0 if valid == len(outcomes) else 1


if __name__ == "__main__":
    sys.exit(main())
