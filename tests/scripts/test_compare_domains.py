"""Tests de scripts/compare_domains.py: la comparación de un dominio aprendido con el de
referencia."""

import contextlib
import io
import json
import tempfile
import unittest
from pathlib import Path

import compare_domains as cd

REFERENCE = """
; Un dominio pequeño, con átomos, una comparación y efectos numéricos.
(define (domain viaje)
  (:requirements :typing :numeric-fluents)
  (:types camion ciudad)
  (:predicates (en ?c - camion ?x - ciudad))
  (:functions (combustible ?c - camion) (distancia ?x ?y - ciudad) (consumo ?c - camion))
  (:action conducir
    :parameters (?c - camion ?origen ?destino - ciudad)
    :precondition (and (en ?c ?origen)
                       (>= (combustible ?c) (* (distancia ?origen ?destino) (consumo ?c))))
    :effect (and (not (en ?c ?origen)) (en ?c ?destino)
                 (decrease (combustible ?c) (* (distancia ?origen ?destino) (consumo ?c)))))
  (:action repostar
    :parameters (?c - camion)
    :precondition (< (combustible ?c) 10)
    :effect (assign (combustible ?c) 10)))
"""

# El mismo dominio como lo escribe PlanMiner: otros nombres, otro orden, > en vez de <=, el
# producto al revés, los números con decimales, en mayúsculas.
EQUIVALENT = """
(define (domain aprendido)
	(:action CONDUCIR
		:parameters (?param_0 - camion ?param_1 - ciudad ?param_2 - ciudad)
		:precondition
			(and
				(<= ( * ( consumo ?param_0 ) ( distancia ?param_1 ?param_2 ) )
					( combustible ?param_0 ) )
				( en ?param_0 ?param_1 )
			)
		:effect
			(and
				( en ?param_0 ?param_2 )
				(and ( not ( en ?param_0 ?param_1 ) ))
				( decrease ( combustible ?param_0 )
					( * ( distancia ?param_1 ?param_2 ) ( consumo ?param_0 ) ) )
			)
	)
	(:action repostar
		:parameters (?param_0 - camion)
		:precondition (and (> 10.000000 ( combustible ?param_0 ) ))
		:effect (and ( assign ( combustible ?param_0 ) 10.0 ))
	)
)
"""


def actions(text: str) -> dict[str, cd.Action]:
    return cd.actions(text)


class Canonical(unittest.TestCase):
    def test_a_domain_matches_itself(self) -> None:
        comparison = cd.compare(actions(REFERENCE), actions(REFERENCE))
        self.assertEqual(comparison.total, cd.Counts(7, 0, 0))
        self.assertEqual(comparison.total.f_score, 1.0)

    def test_equivalent_forms_match(self) -> None:
        comparison = cd.compare(actions(EQUIVALENT), actions(REFERENCE))
        self.assertEqual(comparison.precondition, cd.Counts(3, 0, 0))
        self.assertEqual(comparison.effect, cd.Counts(4, 0, 0))

    def test_equality_is_symmetric(self) -> None:
        learned = (
            "(define (domain d) (:action a :parameters (?x ?y) :precondition (== (f ?x) (g ?y))))"
        )
        reference = (
            "(define (domain d) (:action a :parameters (?p ?q) :precondition (= (g ?q) (f ?p))))"
        )
        self.assertEqual(cd.compare(actions(learned), actions(reference)).total, cd.Counts(1, 0, 0))

    def test_parameters_are_matched_by_position(self) -> None:
        learned = "(define (domain d) (:action a :parameters (?x ?y) :precondition (p ?y ?x)))"
        reference = "(define (domain d) (:action a :parameters (?x ?y) :precondition (p ?x ?y)))"
        self.assertEqual(cd.compare(actions(learned), actions(reference)).total, cd.Counts(0, 1, 1))


class Metrics(unittest.TestCase):
    def test_extra_and_missing_elements(self) -> None:
        learned = """(define (domain d) (:action a :parameters (?x)
                       :precondition (and (p ?x) (q ?x) (r ?x)) :effect (not (p ?x))))"""
        reference = """(define (domain d) (:action a :parameters (?x)
                         :precondition (and (p ?x) (q ?x) (s ?x)) :effect (not (p ?x))))"""
        comparison = cd.compare(actions(learned), actions(reference))
        self.assertEqual(comparison.precondition, cd.Counts(2, 1, 1))
        self.assertAlmostEqual(comparison.precondition.precision, 2 / 3)
        self.assertAlmostEqual(comparison.precondition.recall, 2 / 3)
        self.assertAlmostEqual(comparison.precondition.f_score, 2 / 3)
        self.assertEqual(comparison.total, cd.Counts(3, 1, 1))
        self.assertAlmostEqual(comparison.total.f_score, 0.75)

    def test_extra_and_missing_actions(self) -> None:
        learned = "(define (domain d) (:action a :parameters () :effect (p)))"
        reference = "(define (domain d) (:action b :parameters () :precondition (q) :effect (p)))"
        comparison = cd.compare(actions(learned), actions(reference))
        self.assertEqual([action.name for action in comparison.actions], ["a", "b"])
        self.assertEqual(comparison.total, cd.Counts(0, 1, 2))

    def test_nothing_to_measure(self) -> None:
        self.assertEqual(cd.Counts().f_score, 1.0)
        self.assertEqual(cd.Counts(0, 1, 0).precision, 0.0)
        self.assertEqual(cd.Counts(0, 1, 0).recall, 0.0)
        self.assertEqual(cd.Counts(0, 0, 1).precision, 0.0)
        self.assertEqual(cd.Counts(0, 0, 1).f_score, 0.0)


class Errors(unittest.TestCase):
    def test_unbalanced_parentheses(self) -> None:
        with self.assertRaises(cd.DomainError):
            actions("(define (domain d) (:action a)")
        with self.assertRaises(cd.DomainError):
            actions("(define (domain d)))")

    def test_not_a_domain(self) -> None:
        with self.assertRaises(cd.DomainError):
            actions("(define (problem p))(define (problem q))")

    def test_durative_actions(self) -> None:
        with self.assertRaises(cd.DomainError):
            actions("(define (domain d) (:durative-action a))")


class CommandLine(unittest.TestCase):
    def run_main(self, *args: str) -> tuple[int, str]:
        out = io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
            code = cd.main(list(args))
        return code, out.getvalue()

    def test_report_and_json(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            learned = Path(directory) / "aprendido.pddl"
            reference = Path(directory) / "referencia.pddl"
            learned.write_text(EQUIVALENT)
            reference.write_text(REFERENCE)

            code, text = self.run_main(str(learned), str(reference), "--details")
            self.assertEqual(code, 0)
            self.assertRegex(text, r"total\s+7\s+0\s+0\s+1\.000\s+1\.000\s+1\.000")

            code, text = self.run_main(str(learned), str(reference), "--json")
            self.assertEqual(code, 0)
            self.assertEqual(json.loads(text)["total"]["f_score"], 1.0)

    def test_missing_file(self) -> None:
        code, _ = self.run_main("no-existe.pddl", "tampoco.pddl")
        self.assertEqual(code, 2)


if __name__ == "__main__":
    unittest.main()
