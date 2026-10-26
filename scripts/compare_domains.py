#!/usr/bin/env python3
"""Compara un dominio aprendido con el de referencia: precisión, recall y F-score.

Cada elemento de las precondiciones y los efectos de las acciones (un átomo, su negación, una
comparación numérica o un efecto numérico) cuenta como:

- verdadero positivo (VP): está en el aprendido y en el de referencia;
- falso positivo (FP): sobra, está en el aprendido y no en el de referencia;
- falso negativo (FN): falta, está en el de referencia y no en el aprendido.

Con la suma de todas las acciones, precisión = VP / (VP + FP), recall = VP / (VP + FN) y
F-score, su media armónica; por separado para las precondiciones y los efectos, y juntos. Si no
hay nada que medir (VP + FP o VP + FN es 0), la medida vale 1 si no hay errores y 0 si los hay.

Las acciones se emparejan por su nombre y sus parámetros por su posición: el primer parámetro
del aprendido es el primero del de referencia, se llamen como se llamen. Los tipos no cuentan.
Una acción que sobra cuenta todos sus elementos como falsos positivos, y una que falta, como
falsos negativos.

Antes de comparar, los elementos se escriben de una forma canónica: los nombres en minúsculas;
los números con el mismo formato (1, 1.0 y 1.000000 son el mismo); > y >= como < y <= con los
lados cambiados, y == como =; y los operandos de =, + y * en orden. No se reconocen otras
equivalencias: (distance ?1 ?2) y (distance ?2 ?1) son distintos aunque los datos no los
distingan.

Uso::

    compare_domains.py aprendido.pddl referencia.pddl [--details] [--json]

Códigos de salida: 0 si se han podido comparar, 2 si los argumentos o los dominios no son
válidos.
"""

import argparse
import json
import re
import sys
from collections.abc import Sequence
from dataclasses import dataclass, field
from pathlib import Path

type SExpr = str | list["SExpr"]

COMPARISONS = {"=", "==", "<", "<=", ">", ">="}
NUMERIC_EFFECTS = {"increase", "decrease", "assign", "scale-up", "scale-down"}
COMMUTATIVE = {"+", "*"}


class DomainError(Exception):
    """El fichero no es un dominio PDDL que se pueda comparar."""


def parse(text: str) -> list[SExpr]:
    """Las expresiones S del texto, sin comentarios (de ';' al final de la línea)."""
    tokens = re.findall(r"\(|\)|[^\s()]+", re.sub(r";[^\n]*", "", text))
    stack: list[list[SExpr]] = [[]]
    for token in tokens:
        if token == "(":
            stack.append([])
        elif token == ")":
            if len(stack) == 1:
                raise DomainError("sobra un ')'")
            closed = stack.pop()
            stack[-1].append(closed)
        else:
            stack[-1].append(token.lower())
    if len(stack) != 1:
        raise DomainError("falta un ')'")
    return stack[0]


@dataclass(frozen=True)
class Action:
    name: str
    precondition: frozenset[str]
    effect: frozenset[str]


def _conjuncts(expression: SExpr) -> list[SExpr]:
    """Los elementos de una conjunción, deshaciendo los (and ...) anidados."""
    if isinstance(expression, list):
        if not expression:
            return []
        if expression[0] == "and":
            return [item for part in expression[1:] for item in _conjuncts(part)]
    return [expression]


def _number(token: str) -> str | None:
    try:
        value = float(token)
    except ValueError:
        return None
    return str(int(value)) if value.is_integer() else repr(value)


def _canonical(expression: SExpr, parameters: dict[str, str]) -> str:
    """La forma canónica de un elemento o de una expresión."""
    if isinstance(expression, str):
        if expression in parameters:
            return parameters[expression]
        number = _number(expression)
        return number if number is not None else expression
    if not expression:
        return "()"
    head = expression[0]
    if not isinstance(head, str):
        return "(" + " ".join(_canonical(item, parameters) for item in expression) + ")"
    arguments = [_canonical(item, parameters) for item in expression[1:]]
    if head in COMPARISONS and len(arguments) == 2:
        left, right = arguments
        if head in {">", ">="}:
            head, left, right = ("<" if head == ">" else "<="), right, left
        elif head in {"=", "=="}:
            head, (left, right) = "=", sorted((left, right))
        return f"({head} {left} {right})"
    if head in COMMUTATIVE and len(arguments) == 2:
        arguments = sorted(arguments)
    return "(" + " ".join([head, *arguments]) + ")"


def _parameters(declaration: SExpr) -> dict[str, str]:
    """El nombre canónico de cada parámetro, por su posición: ?0, ?1..."""
    if not isinstance(declaration, list):
        raise DomainError("los parámetros de una acción no son una lista")
    names = [item for item in declaration if isinstance(item, str) and item.startswith("?")]
    return {name: f"?{index}" for index, name in enumerate(names)}


def actions(text: str) -> dict[str, Action]:
    """Las acciones del dominio, por su nombre."""
    definitions = [
        item for item in parse(text) if isinstance(item, list) and item[:1] == ["define"]
    ]
    if len(definitions) != 1:
        raise DomainError("no hay un (define ...)")
    result: dict[str, Action] = {}
    for item in definitions[0][1:]:
        if not isinstance(item, list) or not item or not isinstance(item[0], str):
            continue
        if item[0] == ":durative-action":
            raise DomainError("no se admiten acciones durativas")
        if item[0] != ":action" or len(item) < 2 or not isinstance(item[1], str):
            continue
        name = item[1]
        fields: dict[str, SExpr] = {}
        for index in range(2, len(item) - 1, 2):
            key = item[index]
            if isinstance(key, str):
                fields[key] = item[index + 1]
        parameters = _parameters(fields.get(":parameters", []))
        result[name] = Action(
            name,
            frozenset(
                _canonical(element, parameters)
                for element in _conjuncts(fields.get(":precondition", []))
            ),
            frozenset(
                _canonical(element, parameters) for element in _conjuncts(fields.get(":effect", []))
            ),
        )
    return result


@dataclass
class Counts:
    true_positives: int = 0
    false_positives: int = 0
    false_negatives: int = 0

    def add(self, other: "Counts") -> None:
        self.true_positives += other.true_positives
        self.false_positives += other.false_positives
        self.false_negatives += other.false_negatives

    @property
    def precision(self) -> float:
        predicted = self.true_positives + self.false_positives
        if predicted == 0:
            return 1.0 if self.false_negatives == 0 else 0.0
        return self.true_positives / predicted

    @property
    def recall(self) -> float:
        expected = self.true_positives + self.false_negatives
        if expected == 0:
            return 1.0 if self.false_positives == 0 else 0.0
        return self.true_positives / expected

    @property
    def f_score(self) -> float:
        precision, recall = self.precision, self.recall
        if precision + recall == 0:
            return 0.0
        return 2 * precision * recall / (precision + recall)


def _counts(learned: frozenset[str], reference: frozenset[str]) -> Counts:
    return Counts(len(learned & reference), len(learned - reference), len(reference - learned))


@dataclass
class ActionComparison:
    name: str
    precondition: Counts
    effect: Counts
    #: Los elementos que sobran (FP) y los que faltan (FN), por parte.
    extra: dict[str, list[str]] = field(default_factory=dict)
    missing: dict[str, list[str]] = field(default_factory=dict)


@dataclass
class Comparison:
    actions: list[ActionComparison]
    precondition: Counts
    effect: Counts
    total: Counts


def compare(learned: dict[str, Action], reference: dict[str, Action]) -> Comparison:
    """La comparación de las acciones del aprendido con las del de referencia."""
    empty = frozenset[str]()
    result = Comparison([], Counts(), Counts(), Counts())
    for name in sorted(learned.keys() | reference.keys()):
        mine = learned.get(name)
        theirs = reference.get(name)
        parts = {
            "precondition": (
                mine.precondition if mine else empty,
                theirs.precondition if theirs else empty,
            ),
            "effect": (mine.effect if mine else empty, theirs.effect if theirs else empty),
        }
        comparison = ActionComparison(
            name, _counts(*parts["precondition"]), _counts(*parts["effect"])
        )
        for part, (ours, expected) in parts.items():
            comparison.extra[part] = sorted(ours - expected)
            comparison.missing[part] = sorted(expected - ours)
        result.actions.append(comparison)
        result.precondition.add(comparison.precondition)
        result.effect.add(comparison.effect)
    result.total.add(result.precondition)
    result.total.add(result.effect)
    return result


def _metrics(counts: Counts) -> dict[str, float | int]:
    return {
        "vp": counts.true_positives,
        "fp": counts.false_positives,
        "fn": counts.false_negatives,
        "precision": counts.precision,
        "recall": counts.recall,
        "f_score": counts.f_score,
    }


def report(comparison: Comparison, details: bool) -> str:
    """La comparación como texto: por acción y en total."""
    lines = [f"{'acción':<20} {'precondiciones VP FP FN':>24} {'efectos VP FP FN':>18}"]
    for action in comparison.actions:
        pre, eff = action.precondition, action.effect
        lines.append(
            f"{action.name:<20} {pre.true_positives:>16} {pre.false_positives:>3}"
            f" {pre.false_negatives:>3} {eff.true_positives:>10} {eff.false_positives:>3}"
            f" {eff.false_negatives:>3}"
        )
        if details:
            for part, label in (("precondition", "la precondición"), ("effect", "el efecto")):
                lines.extend(f"    sobra en {label}: {e}" for e in action.extra[part])
                lines.extend(f"    falta en {label}: {e}" for e in action.missing[part])
    lines.append("")
    lines.append(f"{'':<16} {'VP':>4} {'FP':>4} {'FN':>4} {'precisión':>10} {'recall':>8} {'F':>6}")
    for label, counts in (
        ("precondiciones", comparison.precondition),
        ("efectos", comparison.effect),
        ("total", comparison.total),
    ):
        lines.append(
            f"{label:<16} {counts.true_positives:>4} {counts.false_positives:>4}"
            f" {counts.false_negatives:>4} {counts.precision:>10.3f} {counts.recall:>8.3f}"
            f" {counts.f_score:>6.3f}"
        )
    return "\n".join(lines)


def to_json(comparison: Comparison) -> str:
    return json.dumps(
        {
            "actions": [
                {
                    "name": action.name,
                    "precondition": _metrics(action.precondition),
                    "effect": _metrics(action.effect),
                    "extra": action.extra,
                    "missing": action.missing,
                }
                for action in comparison.actions
            ],
            "precondition": _metrics(comparison.precondition),
            "effect": _metrics(comparison.effect),
            "total": _metrics(comparison.total),
        },
        ensure_ascii=False,
        indent=2,
    )


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Compara un dominio aprendido con el de referencia: precisión, recall y "
        "F-score de sus precondiciones y efectos."
    )
    parser.add_argument("learned", type=Path, help="dominio aprendido")
    parser.add_argument("reference", type=Path, help="dominio de referencia")
    parser.add_argument(
        "--details", action="store_true", help="lista los elementos que sobran y los que faltan"
    )
    parser.add_argument("--json", action="store_true", help="escribe el resultado en JSON")
    args = parser.parse_args(argv)
    domains: list[dict[str, Action]] = []
    for path in (args.learned, args.reference):
        try:
            domains.append(actions(path.read_text()))
        except (OSError, DomainError) as error:
            print(f"error: {path}: {error}", file=sys.stderr)
            return 2
    comparison = compare(*domains)
    print(to_json(comparison) if args.json else report(comparison, args.details))
    return 0


if __name__ == "__main__":
    sys.exit(main())
