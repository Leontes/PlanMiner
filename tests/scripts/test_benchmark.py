"""Tests de scripts/benchmark.py: lo que no necesita PDDLDomains, PyPlanner, VAL ni PlanMiner (el
banco completo, con ellos, es el test planner.benchmark)."""

import contextlib
import io
import json
import tempfile
import unittest
from pathlib import Path
from typing import Any

import benchmark as bm
import evaluate_domain as ed

DEFINITION = """
description = "Prueba"
seed = 1
problems = 6
folds = 3

[[select]]
collections = ["ipc-2000/blocks-strips-untyped"]

[[variants]]
name = "complete"
world = "open"
"""


def parse(text: str) -> bm.Definition:
    return bm.parse_definition(text, Path("prueba.toml"))


class Definitions(unittest.TestCase):
    def test_defaults(self) -> None:
        definition = parse(DEFINITION)
        self.assertEqual(definition.name, "prueba")
        self.assertEqual(definition.lock, Path("prueba.lock.json"))
        self.assertEqual(definition.planner, bm.Planner("gbfs-ff-pref", 60, 1024))
        self.assertEqual(definition.planminer, bm.Learner(("original", "improved"), 1800, 2560))
        self.assertEqual(definition.walk["bases"], 1)
        self.assertEqual(definition.planner.limits(), ed.Limits(60, 1024, "gbfs-ff-pref"))

    def test_the_digest_follows_the_text(self) -> None:
        self.assertEqual(parse(DEFINITION).digest, parse(DEFINITION).digest)
        self.assertNotEqual(parse(DEFINITION).digest, parse(DEFINITION + "\n").digest)

    def test_errors(self) -> None:
        for text in (
            DEFINITION.replace("folds = 3", "folds = 1"),
            DEFINITION.replace("problems = 6", "problems = 2"),
            DEFINITION.replace("seed = 1", 'seed = "uno"'),
            "extra = 1\n" + DEFINITION,
            DEFINITION.replace("collections =", "colecciones ="),
            DEFINITION.replace('world = "open"', 'mundo = "open"'),
            DEFINITION + "[planminer]\nconfigurations = ['otra']\n",
            DEFINITION + "[planner]\ntime_limit = 0\n",
            DEFINITION + '[[variants]]\nname = "complete"\n',
            DEFINITION.split("[[select]]")[0],
            "folds = [",
        ):
            with self.subTest(text=text), self.assertRaises(bm.BenchmarkError):
                parse(text)

    def test_the_repository_definitions(self) -> None:
        for path in sorted(bm.BENCHMARKS.glob("*.toml")):
            with self.subTest(path=path.name):
                definition = bm.load_definition(str(path))
                self.assertTrue(definition.lock.is_file(), f"falta {definition.lock.name}")

    def test_names_and_paths(self) -> None:
        self.assertEqual(bm.find_definition("smoke"), bm.BENCHMARKS / "smoke.toml")
        self.assertEqual(bm.find_definition("otro/banco.toml"), Path("otro/banco.toml"))


class Folds(unittest.TestCase):
    PROBLEMS = [f"p{number:02}" for number in range(1, 31)]

    def test_a_partition(self) -> None:
        split = bm.folds(self.PROBLEMS, 3, "1:x")
        self.assertEqual(sorted(name for part in split for name in part), self.PROBLEMS)
        self.assertEqual([len(part) for part in split], [10, 10, 10])
        self.assertEqual([len(part) for part in bm.folds(self.PROBLEMS[:7], 3, "1")], [3, 2, 2])

    def test_reproducible(self) -> None:
        self.assertEqual(bm.folds(self.PROBLEMS, 3, "1:x"), bm.folds(self.PROBLEMS[::-1], 3, "1:x"))
        self.assertNotEqual(bm.folds(self.PROBLEMS, 3, "1:x"), bm.folds(self.PROBLEMS, 3, "2:x"))

    def test_the_same_split_everywhere(self) -> None:
        # Si cambiara (otra versión de Python), los resultados guardados dejarían de valer.
        self.assertEqual(
            bm.folds(["a", "b", "c", "d", "e", "f"], 3, "1"), [["b", "e"], ["a", "d"], ["c", "f"]]
        )


DOMAIN = """(define (domain d)
  (:predicates (at ?x ?y) (road ?x ?y) (up ?x ?y))
  (:functions (distance ?x ?y) (fuel ?t) (total-cost))
  (:action drive
    :parameters (?t ?a ?b)
    :precondition (and (at ?t ?a) (road ?a ?b))
    :effect (and (not (at ?t ?a)) (at ?t ?b) (decrease (fuel ?t) (distance ?a ?b))
                 (increase (total-cost) 1))))"""


def problem(*facts: str) -> str:
    return f"(define (problem p) (:domain d) (:objects a b t) (:init {' '.join(facts)}))"


SYMMETRIC = problem(
    "(at t a)",
    "(at a t)",
    "(road a b)",
    "(road b a)",
    "(up a b)",
    "(= (distance a b) 5)",
    "(= (distance b a) 5)",
    "(= (fuel t) 10)",
)


class Symmetric(unittest.TestCase):
    def test_changed_symbols(self) -> None:
        self.assertEqual(bm.changed_symbols(DOMAIN), {"at", "fuel", "total-cost"})

    def test_static_and_symmetric(self) -> None:
        self.assertEqual(bm.symmetric_symbols(DOMAIN, [SYMMETRIC]), ["distance", "road"])

    def test_in_every_problem(self) -> None:
        other = problem("(road a b)", "(road b a)", "(= (distance a b) 5)", "(= (distance b a) 6)")
        self.assertEqual(bm.symmetric_symbols(DOMAIN, [SYMMETRIC, other]), ["road"])

    def test_with_distinct_arguments(self) -> None:
        self.assertEqual(bm.symmetric_symbols(DOMAIN, [problem("(road a a)")]), [])
        self.assertEqual(bm.symmetric_symbols(DOMAIN, []), [])


class Lock(unittest.TestCase):
    MANIFEST: dict[str, Any] = {
        "pddldomains": "abc",
        "excluded": {"z": "un dominio por problema"},
        "collections": {
            "y": {"problems": {"p01": {}, "p02": {}}, "digest": "d2"},
            "x": {"problems": {"p01": {}}, "digest": "d1"},
        },
    }

    def test_lock_of(self) -> None:
        self.assertEqual(
            bm.lock_of(self.MANIFEST),
            {
                "pddldomains": "abc",
                "collections": {
                    "x": {"problems": 1, "digest": "d1"},
                    "y": {"problems": 2, "digest": "d2"},
                },
                "excluded": {"z": "un dominio por problema"},
            },
        )

    def test_compare(self) -> None:
        lock = bm.lock_of(self.MANIFEST)
        self.assertEqual(bm.compare_lock(lock, lock, {"x", "y"}), [])
        changed = json.loads(json.dumps(lock))
        changed["collections"]["y"]["digest"] = "otra"
        self.assertEqual(bm.compare_lock(lock, changed, {"x", "y"}), ["y: distinta"])
        self.assertEqual(bm.compare_lock(lock, changed, {"x"}), [])
        changed["pddldomains"] = "def"
        self.assertEqual(len(bm.compare_lock(lock, changed, {"x"})), 1)

    def test_digest(self) -> None:
        self.assertEqual(bm.digest({"a": "1", "b": "2"}), bm.digest({"b": "2", "a": "1"}))
        self.assertNotEqual(bm.digest({"a": "1"}), bm.digest({"a": "2"}))


class Outcomes(unittest.TestCase):
    def test_kind(self) -> None:
        path = Path("p01.pddl")
        cases = [
            (ed.Status.VALID, "", "v"),
            (ed.Status.INVALID, "Plan failed to execute", "i"),
            (ed.Status.NO_PLAN, "Resultado: unsolvable (gbfs-ff-pref)", "u"),
            (ed.Status.NO_PLAN, "Resultado: limit_reached (gbfs-ff-pref)", "l"),
            (ed.Status.ERROR, "error: tipo 'saw' no declarado", "e"),
        ]
        for status, detail, expected in cases:
            self.assertEqual(bm.kind(ed.Outcome(path, status, detail)), expected)

    def test_learning_status(self) -> None:
        self.assertEqual(bm.learning_status(0), "ok")
        self.assertEqual(bm.learning_status(-11), "señal 11")
        self.assertEqual(bm.learning_status(1), "error 1")

    def test_validity_text(self) -> None:
        self.assertEqual(bm.validity_text({"v": 3, "i": 1, "x": 2}), "3/4 · i1")
        self.assertEqual(bm.validity_text({"v": 2}), "2/2")
        self.assertEqual(bm.validity_text({"f": 2, "l": 1}), "0/3 · l1 f2")

    def test_planning_domain(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            learned = Path(directory) / "learned.pddl"
            learned.write_text("(define (domain d) (:requirements :typing) (:functions (f)))")
            planning = bm.planning_domain(learned)
            self.assertEqual(planning.name, "planning.pddl")
            self.assertIn("(:requirements :numeric-fluents :typing)", planning.read_text())
            learned.write_text("(define (domain d) (:requirements :typing))")
            self.assertEqual(bm.planning_domain(learned), learned)


def metrics(f_score: float) -> dict[str, dict[str, float]]:
    part = {"vp": 1, "fp": 0, "fn": 0, "precision": 1.0, "recall": f_score, "f_score": f_score}
    return {"precondition": part, "effect": part, "total": part}


def result(fold: int, configuration: str, f_score: float, kinds: str) -> dict[str, Any]:
    return {
        "collection": "ipc-2000/blocks",
        "fold": fold,
        "variant": "complete",
        "configuration": configuration,
        "learning": {"status": "ok" if "f" not in kinds else "señal 11"},
        "exact": metrics(f_score),
        "tolerant": metrics(f_score),
        "outcomes": [{"problem": f"p0{i}", "kind": k} for i, k in enumerate(kinds)],
    }


class Report(unittest.TestCase):
    def test_summarize(self) -> None:
        summary = bm.summarize([result(0, "improved", 0.5, "vvx"), result(1, "improved", 1, "ff")])
        self.assertEqual(summary["folds"], 2)
        self.assertEqual(summary["tolerant"]["total"]["f_score"], {"mean": 0.75, "std": 0.25})
        self.assertEqual(summary["outcomes"]["v"], 2)
        self.assertEqual(summary["outcomes"]["f"], 2)
        self.assertEqual(summary["validity"], 0.5)
        self.assertEqual(summary["learning_failures"], 1)
        self.assertIsNone(bm.summarize([result(0, "improved", 1, "x")])["validity"])

    def test_report(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            definition = bm.parse_definition(DEFINITION, output / "prueba.toml")
            manifest = {
                "definition": definition.digest,
                "pddldomains": "38a868974e6b",
                "pyplanner": "1.0.0",
                "excluded": {"ipc-2011/parc": "un dominio por problema"},
                "failed": {},
                "collections": {"ipc-2000/blocks": {"class": "strips"}},
            }
            (output / "dataset.json").write_text(json.dumps(manifest))
            for fold, (f_score, kinds) in enumerate(((1.0, "vv"), (0.5, "vi"))):
                for configuration in ("original", "improved"):
                    run = (
                        output / "runs" / "ipc-2000" / "blocks" / f"fold{fold + 1}.{configuration}"
                    )
                    run.mkdir(parents=True)
                    data = result(fold, configuration, f_score, kinds)
                    (run / "result.json").write_text(json.dumps(data))

            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(bm.report(definition, output), 0)
            text = (output / "report.md").read_text()
            self.assertIn(
                "| strips (1) | complete | improved | 1.000 | 0.750 | 0.750 | 75 % |", text
            )
            self.assertIn("| ipc-2000/blocks | 1.000 (0.000) | 0.750 (0.250) |", text)
            self.assertIn("3/4 · i1 |", text)
            self.assertIn("- ipc-2011/parc: un dominio por problema.", text)
            self.assertEqual(len(json.loads((output / "report.json").read_text())), 2)

    def test_needs_the_data(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            definition = bm.parse_definition(DEFINITION, Path(directory) / "prueba.toml")
            with self.assertRaises(bm.BenchmarkError):
                bm.report(definition, Path(directory))


if __name__ == "__main__":
    unittest.main()
