#!/usr/bin/env python3
"""El banco de pruebas de PlanMiner: dominios de PDDLDomains, trazas y validación cruzada.

Un banco se define en un TOML (los del repositorio están en benchmarks/; default.toml explica
cada clave). Tiene tres pasos, cada uno con su orden:

- ``generate``: elige las colecciones del catálogo de PDDLDomains (vendor/PDDLDomains) y, de cada
  una, genera los problemas con su plan (paseos aleatorios en las clásicas; el generador de la
  colección y PyPlanner en las numéricas), comprueba con PyPlanner y VAL qué problemas resuelve
  el dominio de referencia y escribe una traza de cada plan por variante. Las huellas de lo
  generado se comparan con las de <banco>.lock.json, junto a la definición: si no coinciden, la
  generación no es la misma (otra versión de PDDLDomains o de PyPlanner).
- ``run``: la validación cruzada. Los problemas de cada colección se reparten en k particiones;
  con cada una, PlanMiner aprende el dominio con las trazas de las demás (por variante y
  configuración), se compara con el de referencia (scripts/compare_domains.py, exacto y
  tolerante) y PyPlanner planifica con él los problemas de la partición que resuelve la
  referencia; VAL valida los planes con el de referencia. Cada resultado se guarda: si se
  interrumpe, ``run`` sigue donde se quedó.
- ``report``: las tablas de precisión, recall, F y validez de los planes.

Todo queda en el directorio de salida (por defecto, build/benchmark/<banco>): data/ (problemas,
planes y trazas), dataset.json, runs/ (dominios aprendidos y resultados), report.md y
report.json.

Se ejecuta con el Python de .venv (scripts/setup_python.sh), que tiene PyPlanner y pddldomains, y
necesita VAL (scripts/build_val.sh) y el ejecutable de PlanMiner. Cada trabajo en paralelo usa
hasta los límites de memoria de la definición, y todos tienen que caber en el 80 % de la memoria
disponible.

Uso::

    benchmark.py generate [banco] [--output DIR] [--jobs N] [--collections PATRÓN...]
        [--update-lock]
    benchmark.py run [banco] [--output DIR] [--jobs N] [--collections PATRÓN...] [--force]
    benchmark.py report [banco] [--output DIR]

Códigos de salida: 0 si todo va bien; 1 si las huellas no coinciden con el .lock.json; 2 si los
argumentos, la definición o las herramientas no son válidos.
"""

import argparse
import fnmatch
import hashlib
import json
import math
import random
import shutil
import statistics
import subprocess
import sys
import time
import tomllib
from collections.abc import Callable, Iterable, Sequence
from concurrent.futures import ThreadPoolExecutor
from dataclasses import asdict, dataclass
from importlib.metadata import PackageNotFoundError, version
from pathlib import Path
from typing import Any

import compare_domains as cd
import evaluate_domain as ed
from generate_traces import OPTIONS as TRACE_OPTIONS

ROOT = Path(__file__).resolve().parent.parent
BENCHMARKS = ROOT / "benchmarks"
PDDLDOMAINS = ROOT / "vendor" / "PDDLDomains"
MIB = 1024 * 1024

#: Los filtros de [[select]] y su opción de `pddldomains dataset`.
SELECT_OPTIONS = {
    "collections": "--collection",
    "exclude-collections": "--exclude-collection",
    "types": "--type",
    "exclude-types": "--exclude-type",
    "requirements": "--requirement",
    "exclude-requirements": "--exclude-requirement",
    "levels": "--level",
    "min-level": "--min-level",
    "max-level": "--max-level",
}
#: Las configuraciones de PlanMiner y sus opciones.
CONFIGURATIONS = {"original": ["--original"], "improved": []}
#: Lo que pasa con cada problema de prueba: v válido, i inválido (VAL lo rechaza), u sin solución,
#: l límite del planificador, e error (PyPlanner rechaza el dominio o el problema), f sin dominio
#: (PlanMiner no ha aprendido ninguno) y x fuera (la referencia no lo resuelve).
KINDS = "viulefx"


class BenchmarkError(Exception):
    pass


# --- La definición ---------------------------------------------------------------------------


@dataclass(frozen=True)
class Planner:
    config: str
    time_limit: int
    memory_limit: int

    def limits(self) -> ed.Limits:
        return ed.Limits(self.time_limit, self.memory_limit, self.config)


@dataclass(frozen=True)
class Learner:
    configurations: tuple[str, ...]
    time_limit: int
    memory_limit: int


@dataclass(frozen=True)
class Definition:
    name: str
    path: Path
    digest: str
    description: str
    seed: int
    problems: int
    folds: int
    selects: list[dict[str, Any]]
    walk: dict[str, Any]
    generators: dict[str, dict[str, Any]]
    planner: Planner
    planminer: Learner
    variants: list[dict[str, Any]]

    @property
    def lock(self) -> Path:
        return self.path.with_suffix(".lock.json")


def _check_keys(table: dict[str, Any], allowed: Iterable[str], where: str) -> None:
    unknown = set(table) - set(allowed)
    if unknown:
        raise BenchmarkError(f"{where}: claves desconocidas {sorted(unknown)}")


def _integer(
    table: dict[str, Any], key: str, where: str, minimum: int, default: int | None = None
) -> int:
    value = table.get(key, default)
    if not isinstance(value, int) or isinstance(value, bool) or value < minimum:
        raise BenchmarkError(f"{where}: '{key}' tiene que ser un entero de al menos {minimum}")
    return value


def find_definition(name: str) -> Path:
    """La definición: una ruta a un TOML o el nombre de uno de benchmarks/."""
    path = Path(name)
    if path.suffix == ".toml" or path.exists():
        return path
    return BENCHMARKS / f"{name}.toml"


def parse_definition(text: str, path: Path) -> Definition:
    try:
        data = tomllib.loads(text)
    except tomllib.TOMLDecodeError as error:
        raise BenchmarkError(f"{path}: {error}") from error
    _check_keys(
        data,
        {"description", "seed", "problems", "folds", "select", "walk", "generators"}
        | {"planner", "planminer", "variants"},
        str(path),
    )
    folds = _integer(data, "folds", str(path), 2)
    problems = _integer(data, "problems", str(path), folds)

    selects = data.get("select", [])
    if not selects:
        raise BenchmarkError(f"{path}: falta algún [[select]]")
    for select in selects:
        _check_keys(select, SELECT_OPTIONS, f"{path}: [[select]]")

    walk = {"bases": 1, "mode": "goal", "steps": "10..40", "goals": "2..6"} | data.get("walk", {})
    _check_keys(walk, {"bases", "mode", "steps", "goals"}, f"{path}: [walk]")
    _integer(walk, "bases", f"{path}: [walk]", 1)

    planner = data.get("planner", {})
    _check_keys(planner, {"config", "time_limit", "memory_limit"}, f"{path}: [planner]")
    learner = data.get("planminer", {})
    _check_keys(learner, {"configurations", "time_limit", "memory_limit"}, f"{path}: [planminer]")
    configurations = tuple(learner.get("configurations", CONFIGURATIONS))
    unknown = set(configurations) - set(CONFIGURATIONS)
    if unknown or not configurations:
        raise BenchmarkError(
            f"{path}: [planminer] configurations tiene que ser de {sorted(CONFIGURATIONS)}"
        )

    variants = data.get("variants", [])
    names = [variant.get("name") for variant in variants]
    if not variants or None in names or len(set(names)) != len(names):
        raise BenchmarkError(f"{path}: hace falta algún [[variants]], cada uno con su nombre")
    for variant in variants:
        _check_keys(variant, {"name", *TRACE_OPTIONS}, f"{path}: variante {variant['name']}")

    return Definition(
        name=path.stem,
        path=path,
        digest=hashlib.sha256(text.encode()).hexdigest(),
        description=str(data.get("description", "")),
        seed=_integer(data, "seed", str(path), 0),
        problems=problems,
        folds=folds,
        selects=selects,
        walk=walk,
        generators=data.get("generators", {}),
        planner=Planner(
            str(planner.get("config", "gbfs-ff-pref")),
            _integer(planner, "time_limit", f"{path}: [planner]", 1, 60),
            _integer(planner, "memory_limit", f"{path}: [planner]", 1, 1024),
        ),
        planminer=Learner(
            configurations,
            _integer(learner, "time_limit", f"{path}: [planminer]", 1, 1800),
            _integer(learner, "memory_limit", f"{path}: [planminer]", 1, 2560),
        ),
        variants=variants,
    )


def load_definition(name: str) -> Definition:
    path = find_definition(name)
    try:
        text = path.read_text(encoding="utf-8")
    except OSError as error:
        raise BenchmarkError(f"no se puede leer la definición {path}: {error.strerror}") from error
    return parse_definition(text, path)


# --- Las herramientas ------------------------------------------------------------------------


@dataclass(frozen=True)
class Tools:
    validate: Path
    planminer: Path | None = None


def run_command(command: Sequence[str], cwd: Path | None = None) -> str:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        raise BenchmarkError(
            f"{' '.join(command)} ha terminado con {result.returncode}\n"
            f"{(result.stderr or result.stdout).strip()[-2000:]}"
        )
    return result.stdout


def pddldomains(*arguments: str, cwd: Path | None = None) -> str:
    return run_command([sys.executable, "-m", "pddldomains", "--lang", "es", *arguments], cwd)


def _available_memory() -> int:
    """La memoria disponible, en bytes (0 si no se sabe)."""
    try:
        for line in Path("/proc/meminfo").read_text().splitlines():
            if line.startswith("MemAvailable:"):
                return int(line.split()[1]) * 1024
    except OSError:
        pass
    return 0


def check_memory(jobs: int, per_job_mb: int) -> None:
    """Los trabajos tienen que caber en el 80 % de la memoria disponible: en WSL, agotarla
    congela todo."""
    available = _available_memory()
    if available and jobs * per_job_mb * MIB > 0.8 * available:
        raise BenchmarkError(
            f"{jobs} trabajos de hasta {per_job_mb} MB no caben en el 80 % de la memoria "
            f"disponible ({available // MIB} MB): use menos (--jobs)"
        )


def kind(outcome: ed.Outcome) -> str:
    if outcome.status == ed.Status.VALID:
        return "v"
    if outcome.status == ed.Status.INVALID:
        return "i"
    if outcome.status == ed.Status.NO_PLAN:
        return "u" if "unsolvable" in outcome.detail else "l"
    return "e"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


# --- La selección de colecciones -------------------------------------------------------------


@dataclass(frozen=True)
class Collection:
    id: str
    numeric: bool
    #: Los problemas más pequeños de la colección: el punto de partida de los paseos.
    bases: tuple[str, ...]


def select_collections(definition: Definition) -> tuple[list[Collection], dict[str, str]]:
    """Las colecciones de los [[select]] y las descartadas, con el motivo."""
    tasks: dict[str, list[dict[str, Any]]] = {}
    for select in definition.selects:
        command = ["dataset", "--format", "json", "--order", "smallest"]
        command += ["--per-collection", str(definition.walk["bases"])]
        for key, value in select.items():
            for item in value if isinstance(value, list) else [value]:
                command += [SELECT_OPTIONS[key], str(item)]
        for task in json.loads(pddldomains(*command))["tasks"]:
            known = tasks.setdefault(task["collection"], [])
            if task["problem"] not in {other["problem"] for other in known}:
                known.append(task)
    selected, excluded = [], {}
    for identifier in sorted(tasks):
        found = tasks[identifier]
        if any(Path(task["domain_file"]).name != "domain.pddl" for task in found):
            excluded[identifier] = "un dominio por problema (PlanMiner aprende uno)"
            continue
        numeric = any("numeric" in task["tags"] for task in found)
        bases = tuple(task["problem"] for task in found)
        selected.append(Collection(identifier, numeric, bases))
    return selected, excluded


# --- Los símbolos simétricos -----------------------------------------------------------------


def _definition_items(text: str) -> list[cd.SExpr]:
    definitions = [
        item for item in cd.parse(text) if isinstance(item, list) and item[:1] == ["define"]
    ]
    if len(definitions) != 1:
        raise cd.DomainError("no hay un (define ...)")
    return definitions[0][1:]


def _effect_targets(effect: cd.SExpr) -> set[str]:
    """Los símbolos que cambia un efecto."""
    if not isinstance(effect, list) or not effect or not isinstance(effect[0], str):
        return set()
    head = effect[0]
    if head == "and":
        return set().union(*(_effect_targets(part) for part in effect[1:]))
    if head in {"forall", "when"}:
        return _effect_targets(effect[2]) if len(effect) > 2 else set()
    if head == "not":
        return _effect_targets(effect[1]) if len(effect) > 1 else set()
    if head in cd.NUMERIC_EFFECTS:
        target = effect[1] if len(effect) > 1 else []
        return (
            {target[0]}
            if isinstance(target, list) and target and isinstance(target[0], str)
            else set()
        )
    return {head}


def changed_symbols(domain: str) -> set[str]:
    """Los predicados y las funciones que cambian las acciones del dominio."""
    changed: set[str] = set()
    for item in _definition_items(domain):
        if isinstance(item, list) and item[:1] == [":action"]:
            for index in range(2, len(item) - 1, 2):
                if item[index] == ":effect":
                    changed |= _effect_targets(item[index + 1])
    return changed


def _binary_facts(problem: str) -> dict[str, dict[tuple[str, str], float | None]]:
    """Los hechos iniciales de dos argumentos: por símbolo, los argumentos y su valor (None en
    un átomo)."""
    facts: dict[str, dict[tuple[str, str], float | None]] = {}
    for item in _definition_items(problem):
        if not isinstance(item, list) or item[:1] != [":init"]:
            continue
        for fact in item[1:]:
            if not isinstance(fact, list):
                continue
            value: float | None = None
            if fact[:1] == ["="] and len(fact) == 3 and isinstance(fact[2], str):
                try:
                    value = float(fact[2])
                except ValueError:
                    continue
                fact = fact[1]
            if (
                isinstance(fact, list)
                and len(fact) == 3
                and all(isinstance(part, str) for part in fact)
            ):
                symbol, first, second = (str(part) for part in fact)
                facts.setdefault(symbol, {})[first, second] = value
    return facts


def symmetric_symbols(domain: str, problems: Iterable[str]) -> list[str]:
    """Los predicados y las funciones de dos argumentos que no cambia ninguna acción y cuyo valor
    no depende del orden de sus argumentos en ningún problema (con algún par de argumentos
    distintos)."""
    changed = changed_symbols(domain)
    symmetric: set[str] | None = None
    evidence: set[str] = set()
    for problem in problems:
        here = set()
        for symbol, facts in _binary_facts(problem).items():
            if symbol in changed:
                continue
            if all(
                (second, first) in facts and facts[second, first] == value
                for (first, second), value in facts.items()
            ):
                here.add(symbol)
                if any(first != second for first, second in facts):
                    evidence.add(symbol)
        symmetric = here if symmetric is None else symmetric & here
    return sorted((symmetric or set()) & evidence)


# --- La generación ---------------------------------------------------------------------------


def _generate(
    generator: str, parameters: dict[str, Any], count: int, seed: int, directory: Path
) -> list[dict[str, Any]]:
    """Genera count problemas en directory con un generador de PDDLDomains; su lista, de
    generation.json."""
    options = [item for key, value in parameters.items() for item in ("-p", f"{key}={value}")]
    options += ["-n", str(count), "-s", str(seed), "-o", str(directory), "--force", "--no-check"]
    pddldomains("generate", generator, *options)
    problems: list[dict[str, Any]] = json.loads((directory / "generation.json").read_text())[
        "problems"
    ]
    return problems


def _walk(collection: Collection, definition: Definition, directory: Path) -> dict[str, str]:
    """Los problemas de una colección clásica, por paseos aleatorios: su base."""
    walk = definition.walk
    parameters = {key: walk[key] for key in ("mode", "steps", "goals")}
    parameters = {"collection": collection.id, **parameters, "base": ",".join(collection.bases)}
    per_base = math.ceil(definition.problems / len(collection.bases))
    problems: dict[str, str] = {}
    for entry in _generate("random-walk", parameters, per_base, definition.seed, directory):
        name = Path(entry["file"]).stem
        if len(problems) < definition.problems:
            problems[name] = str(entry["params"]["base"])
        else:
            for suffix in (".pddl", ".plan"):
                (directory / f"{name}{suffix}").unlink(missing_ok=True)
    return problems


def _numeric(
    collection: Collection, definition: Definition, directory: Path, tools: Tools
) -> tuple[list[str], dict[str, str]]:
    """Los problemas de una colección numérica: los primeros que resuelve PyPlanner de los que da
    su generador, y los descartados con el motivo."""
    candidates = definition.problems + math.ceil(definition.problems / 2)
    parameters = definition.generators.get(collection.id, {})
    generated = _generate(collection.id, parameters, candidates, definition.seed, directory)
    kept: list[str] = []
    dropped: dict[str, str] = {}
    domain = directory / "domain.pddl"
    limits = definition.planner.limits()
    for entry in generated:
        problem = directory / entry["file"]
        if len(kept) < definition.problems:
            outcome = ed.evaluate_problem(
                domain, domain, problem, tools.validate, limits, directory
            )
            if outcome.status == ed.Status.VALID:
                kept.append(problem.stem)
                continue
            dropped[problem.stem] = f"{kind(outcome)}: {outcome.detail}"
        problem.unlink()
        (directory / f"{problem.stem}.plan").unlink(missing_ok=True)
    return kept, dropped


def _reference(directory: Path, problem: str, definition: Definition, tools: Tools) -> str:
    """Lo que pasa al planificar el problema con el dominio de referencia (sin tocar su plan)."""
    domain = directory / "domain.pddl"
    workdir = directory / "reference"
    workdir.mkdir(exist_ok=True)
    outcome = ed.evaluate_problem(
        domain,
        domain,
        directory / f"{problem}.pddl",
        tools.validate,
        definition.planner.limits(),
        workdir,
    )
    return kind(outcome)


def _check_plan(directory: Path, problem: str, tools: Tools) -> None:
    output = run_command(
        [str(tools.validate), "domain.pddl", f"{problem}.pddl", f"{problem}.plan"], directory
    )
    if "Plan valid" not in output:
        raise BenchmarkError(f"{directory / problem}.plan: VAL no lo da por válido")


def _trace(directory: Path, problem: str, variant: dict[str, Any], planner: list[str]) -> None:
    """La traza del plan de un problema, en traces/<problema>.<variante>.json. Las rutas son
    relativas, para que la traza no dependa de dónde se genere."""
    command = ["trace", "domain.pddl", f"{problem}.pddl", f"{problem}.plan"]
    command += ["-o", f"traces/{problem}.{variant['name']}.json", *planner]
    for key, value in variant.items():
        if key != "name":
            command += [TRACE_OPTIONS[key], str(value)]
    pddldomains(*command, cwd=directory)


def digest(files: dict[str, str]) -> str:
    """La huella de un conjunto de ficheros, a partir de las suyas."""
    lines = "".join(f"{name} {files[name]}\n" for name in sorted(files))
    return hashlib.sha256(lines.encode()).hexdigest()


def generate_collection(
    collection: Collection, definition: Definition, output: Path, tools: Tools
) -> dict[str, Any]:
    """Los problemas, los planes y las trazas de una colección, y su entrada de dataset.json."""
    directory = output / "data" / collection.id
    if directory.exists():
        shutil.rmtree(directory)
    directory.mkdir(parents=True)
    entry: dict[str, Any] = {"class": "numeric" if collection.numeric else "strips"}
    problems: dict[str, dict[str, str]] = {}
    if collection.numeric:
        kept, dropped = _numeric(collection, definition, directory, tools)
        problems = {name: {"reference": "v"} for name in kept}
        entry |= {"source": "generator", "dropped": dropped}
        planner = ["--planner", f"pyplanner {_version('pyplanner')}"]
        planner += ["--planner-config", definition.planner.config]
    else:
        for name, base in _walk(collection, definition, directory).items():
            _check_plan(directory, name, tools)
            problems[name] = {
                "base": base,
                "reference": _reference(directory, name, definition, tools),
            }
        entry |= {"source": "random-walk", "bases": list(collection.bases)}
        planner = ["--planner", "random-walk"]
        shutil.rmtree(directory / "reference", ignore_errors=True)

    (directory / "traces").mkdir()
    for name in problems:
        for variant in definition.variants:
            _trace(directory, name, variant, planner)

    domain = (directory / "domain.pddl").read_text()
    texts = [(directory / f"{name}.pddl").read_text() for name in problems]
    files = {"domain.pddl": sha256(directory / "domain.pddl")}
    for name in problems:
        for suffix in (".pddl", ".plan"):
            files[f"{name}{suffix}"] = sha256(directory / f"{name}{suffix}")
    for trace in sorted((directory / "traces").iterdir()):
        files[f"traces/{trace.name}"] = sha256(trace)
    entry |= {
        "problems": problems,
        "symmetric": symmetric_symbols(domain, texts),
        "files": files,
        "digest": digest(files),
    }
    return entry


def _version(package: str) -> str:
    try:
        return version(package)
    except PackageNotFoundError:
        return "?"


def _pddldomains_commit() -> str:
    try:
        return run_command(["git", "-C", str(PDDLDOMAINS), "rev-parse", "HEAD"]).strip()
    except (BenchmarkError, OSError):
        return "?"


def lock_of(manifest: dict[str, Any]) -> dict[str, Any]:
    """Lo que tiene que ser igual en cada generación: las huellas de cada colección y las
    descartadas."""
    return {
        "pddldomains": manifest["pddldomains"],
        "collections": {
            identifier: {"problems": len(entry["problems"]), "digest": entry["digest"]}
            for identifier, entry in sorted(manifest["collections"].items())
        },
        "excluded": manifest["excluded"],
    }


def compare_lock(expected: dict[str, Any], actual: dict[str, Any], only: set[str]) -> list[str]:
    """Las diferencias entre el lock guardado y el de la generación, en las colecciones de only."""
    differences = []
    if expected.get("pddldomains") != actual["pddldomains"]:
        differences.append(
            f"PDDLDomains: {expected.get('pddldomains')} en el lock, {actual['pddldomains']} ahora"
        )
    saved = expected.get("collections", {})
    for identifier in sorted(only):
        if saved.get(identifier) != actual["collections"].get(identifier):
            differences.append(f"{identifier}: distinta")
    return differences


def _filter(identifiers: Iterable[str], patterns: Sequence[str]) -> list[str]:
    return [
        identifier
        for identifier in identifiers
        if not patterns or any(fnmatch.fnmatch(identifier, pattern) for pattern in patterns)
    ]


def _parallel[T, R](function: Callable[[T], R], items: Sequence[T], jobs: int) -> list[R]:
    with ThreadPoolExecutor(max(1, jobs)) as pool:
        return list(pool.map(function, items))


def generate(
    definition: Definition,
    output: Path,
    tools: Tools,
    jobs: int,
    patterns: Sequence[str],
    update_lock: bool,
) -> int:
    check_memory(jobs, definition.planner.memory_limit + 512)
    selected, excluded = select_collections(definition)
    chosen = set(_filter((c.id for c in selected), patterns))
    collections = [collection for collection in selected if collection.id in chosen]
    if not collections:
        raise BenchmarkError("ninguna colección cumple los filtros")

    manifest_file = output / "dataset.json"
    manifest: dict[str, Any] = {}
    if manifest_file.exists():
        manifest = json.loads(manifest_file.read_text())
        if manifest.get("definition") != definition.digest:
            manifest = {}
    manifest |= {
        "benchmark": definition.name,
        "definition": definition.digest,
        "pddldomains": _pddldomains_commit(),
        "pyplanner": _version("pyplanner"),
        "excluded": excluded,
    }
    manifest.setdefault("collections", {})
    manifest.setdefault("failed", {})

    def job(collection: Collection) -> tuple[str, dict[str, Any] | str]:
        try:
            entry = generate_collection(collection, definition, output, tools)
        except (BenchmarkError, cd.DomainError, OSError, KeyError, ValueError) as error:
            print(f"{collection.id}: error", flush=True)
            return collection.id, str(error)
        print(f"{collection.id}: {len(entry['problems'])} problemas", flush=True)
        return collection.id, entry

    output.mkdir(parents=True, exist_ok=True)
    for identifier, result in _parallel(job, collections, jobs):
        if isinstance(result, str):
            manifest["collections"].pop(identifier, None)
            manifest["failed"][identifier] = result
        else:
            manifest["failed"].pop(identifier, None)
            manifest["collections"][identifier] = result
    manifest_file.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n")
    for identifier, reason in excluded.items():
        print(f"{identifier}: descartada, {reason}")
    for identifier, error in manifest["failed"].items():
        print(f"{identifier}: no se ha podido generar\n{error}", file=sys.stderr)

    lock = lock_of(manifest)
    saved = json.loads(definition.lock.read_text()) if definition.lock.exists() else None
    if update_lock or saved is None:
        if saved is not None:
            # Solo cambian las huellas de las colecciones generadas ahora.
            collections = saved["collections"] | {
                identifier: lock["collections"][identifier]
                for identifier in chosen
                if identifier in lock["collections"]
            }
            lock["collections"] = dict(sorted(collections.items()))
        definition.lock.write_text(json.dumps(lock, indent=2) + "\n")
        print(f"huellas guardadas en {definition.lock}")
        return 0
    differences = compare_lock(saved, lock, chosen)
    for difference in differences:
        print(f"distinta de {definition.lock.name}: {difference}")
    return 1 if differences else 0


# --- La validación cruzada -------------------------------------------------------------------


def folds(problems: Iterable[str], k: int, seed: str) -> list[list[str]]:
    """Las k particiones de los problemas: un orden al azar repartido por turnos. Se baraja con
    random(), cuya secuencia Python garantiza entre versiones (shuffle no)."""
    order = sorted(problems)
    rng = random.Random(seed)
    for index in range(len(order) - 1, 0, -1):
        other = int(rng.random() * (index + 1))
        order[index], order[other] = order[other], order[index]
    return [sorted(order[part::k]) for part in range(k)]


@dataclass(frozen=True)
class Task:
    collection: str
    fold: int
    variant: str
    configuration: str

    @property
    def name(self) -> str:
        return f"fold{self.fold + 1}.{self.variant}.{self.configuration}"


def learning_status(returncode: int) -> str:
    if returncode == 0:
        return "ok"
    if returncode < 0:
        return f"señal {-returncode}"
    return f"error {returncode}"


def learn(
    planminer: Path, configuration: str, traces: Sequence[Path], learned: Path, settings: Learner
) -> dict[str, Any]:
    """Aprende el dominio con PlanMiner, con límite de tiempo y de memoria."""
    command = ["prlimit", f"--data={(settings.memory_limit + 256) * MIB}", "--", str(planminer)]
    command += [*CONFIGURATIONS[configuration], "-m", str(settings.memory_limit)]
    command += ["-o", str(learned), *map(str, traces)]
    start = time.monotonic()
    try:
        result = subprocess.run(
            command, capture_output=True, text=True, check=False, timeout=settings.time_limit
        )
        status, log = learning_status(result.returncode), result.stdout + result.stderr
    except subprocess.TimeoutExpired:
        status, log = "límite de tiempo", ""
    seconds = round(time.monotonic() - start, 1)
    learned.with_suffix(".log").write_text(log)
    if status == "ok" and not learned.exists():
        status = "sin dominio"
    return {"status": status, "seconds": seconds, "detail": log.strip()[-500:]}


def planning_domain(learned: Path) -> Path:
    """El dominio con el que se planifica: el aprendido, con :numeric-fluents si tiene funciones y
    no lo declara (la configuración original no lo escribe y PyPlanner lo rechaza)."""
    text = learned.read_text()
    if "(:functions" not in text or ":numeric-fluents" in text:
        return learned
    planning = learned.with_name("planning.pddl")
    planning.write_text(text.replace("(:requirements", "(:requirements :numeric-fluents", 1))
    return planning


def metrics(comparison: cd.Comparison) -> dict[str, dict[str, float]]:
    return {
        part: {
            "vp": counts.true_positives,
            "fp": counts.false_positives,
            "fn": counts.false_negatives,
            "precision": counts.precision,
            "recall": counts.recall,
            "f_score": counts.f_score,
        }
        for part, counts in (
            ("precondition", comparison.precondition),
            ("effect", comparison.effect),
            ("total", comparison.total),
        )
    }


def run_task(
    task: Task, definition: Definition, entry: dict[str, Any], output: Path, tools: Tools
) -> dict[str, Any]:
    """Un aprendizaje de la validación cruzada: el dominio, su comparación y sus planes."""
    directory = output / "runs" / task.collection / task.name
    result_file = directory / "result.json"
    key = {
        "dataset": entry["digest"],
        "folds": definition.folds,
        "seed": definition.seed,
        "planner": asdict(definition.planner),
        "planminer": asdict(definition.planminer) | {"configurations": None},
    }
    if result_file.exists():
        saved: dict[str, Any] = json.loads(result_file.read_text())
        if saved.get("key") == key:
            return saved
    if directory.exists():
        shutil.rmtree(directory)
    directory.mkdir(parents=True)

    data = output / "data" / task.collection
    split = folds(entry["problems"], definition.folds, f"{definition.seed}:{task.collection}")
    test = split[task.fold]
    train = [name for name in sorted(entry["problems"]) if name not in test]
    traces = [data / "traces" / f"{name}.{task.variant}.json" for name in train]
    learned = directory / "learned.pddl"
    assert tools.planminer is not None
    learning = learn(tools.planminer, task.configuration, traces, learned, definition.planminer)

    reference = cd.actions((data / "domain.pddl").read_text())
    actions: dict[str, cd.Action] = {}
    if learning["status"] == "ok":
        try:
            actions = cd.actions(learned.read_text())
        except cd.DomainError as error:
            learning |= {"status": "dominio no válido", "detail": str(error)}
    symmetric = frozenset(entry["symmetric"])

    outcomes = []
    for name in test:
        if entry["problems"][name]["reference"] != "v":
            outcomes.append(
                {"problem": name, "kind": "x", "detail": "la referencia no lo resuelve"}
            )
        elif learning["status"] != "ok":
            outcomes.append({"problem": name, "kind": "f", "detail": learning["status"]})
        else:
            outcome = ed.evaluate_problem(
                planning_domain(learned),
                data / "domain.pddl",
                data / f"{name}.pddl",
                tools.validate,
                definition.planner.limits(),
                directory,
            )
            outcomes.append({"problem": name, "kind": kind(outcome), "detail": outcome.detail})

    result = asdict(task) | {
        "key": key,
        "train": train,
        "test": test,
        "learning": learning,
        "exact": metrics(cd.compare(actions, reference)),
        "tolerant": metrics(cd.compare(actions, reference, True, symmetric)),
        "outcomes": outcomes,
    }
    result_file.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n")
    return result


def load_manifest(output: Path, definition: Definition) -> dict[str, Any]:
    try:
        manifest: dict[str, Any] = json.loads((output / "dataset.json").read_text())
    except OSError as error:
        raise BenchmarkError(
            f"no hay datos en {output}: ejecute antes 'benchmark.py generate {definition.name}'"
        ) from error
    if manifest.get("definition") != definition.digest:
        raise BenchmarkError(
            f"los datos de {output} son de otra versión de la definición: vuelva a generarlos"
        )
    return manifest


def run(
    definition: Definition,
    output: Path,
    tools: Tools,
    jobs: int,
    patterns: Sequence[str],
    force: bool,
) -> int:
    check_memory(
        jobs, max(definition.planminer.memory_limit + 256, definition.planner.memory_limit + 512)
    )
    if tools.planminer is None or not tools.planminer.is_file():
        raise BenchmarkError(f"no está el ejecutable de PlanMiner: {tools.planminer}")
    manifest = load_manifest(output, definition)
    collections = _filter(sorted(manifest["collections"]), patterns)
    tasks = [
        Task(collection, fold, variant["name"], configuration)
        for collection in collections
        for fold in range(definition.folds)
        for variant in definition.variants
        for configuration in definition.planminer.configurations
    ]
    if force:
        for task in tasks:
            (output / "runs" / task.collection / task.name / "result.json").unlink(missing_ok=True)

    def job(task: Task) -> None:
        result = run_task(task, definition, manifest["collections"][task.collection], output, tools)
        kinds = "".join(outcome["kind"] for outcome in result["outcomes"])
        f_score = result["tolerant"]["total"]["f_score"]
        print(
            f"{task.collection} {task.name}: {result['learning']['status']}, F {f_score:.3f}, "
            f"planes {kinds}",
            flush=True,
        )

    _parallel(job, tasks, jobs)
    return 0


# --- El informe ------------------------------------------------------------------------------


def _mean(values: Sequence[float]) -> float:
    return statistics.fmean(values) if values else 0.0


def _deviation(values: Sequence[float]) -> float:
    return statistics.pstdev(values) if len(values) > 1 else 0.0


def summarize(results: Sequence[dict[str, Any]]) -> dict[str, Any]:
    """Lo de las particiones de una colección, variante y configuración: la media y la desviación
    típica de cada medida y lo que ha pasado con cada problema de prueba."""
    summary: dict[str, Any] = {"folds": len(results)}
    for mode in ("tolerant", "exact"):
        summary[mode] = {
            part: {
                measure: {
                    "mean": _mean([result[mode][part][measure] for result in results]),
                    "std": _deviation([result[mode][part][measure] for result in results]),
                }
                for measure in ("precision", "recall", "f_score")
            }
            for part in ("precondition", "effect", "total")
        }
    kinds = [outcome["kind"] for result in results for outcome in result["outcomes"]]
    summary["outcomes"] = {letter: kinds.count(letter) for letter in KINDS}
    tested = len(kinds) - kinds.count("x")
    summary["validity"] = kinds.count("v") / tested if tested else None
    summary["learning_failures"] = sum(
        1 for result in results if result["learning"]["status"] != "ok"
    )
    return summary


def validity_text(outcomes: dict[str, int]) -> str:
    tested = sum(outcomes.values()) - outcomes.get("x", 0)
    failures = " ".join(
        f"{letter}{outcomes[letter]}" for letter in KINDS[1:-1] if outcomes.get(letter)
    )
    return f"{outcomes.get('v', 0)}/{tested}" + (f" · {failures}" if failures else "")


def _cell(measure: dict[str, float]) -> str:
    return f"{measure['mean']:.3f} ({measure['std']:.3f})"


def report(definition: Definition, output: Path) -> int:
    manifest = load_manifest(output, definition)
    results = [
        json.loads(path.read_text()) for path in sorted((output / "runs").glob("*/*/*/result.json"))
    ]
    if not results:
        raise BenchmarkError(f"no hay resultados en {output / 'runs'}: ejecute antes 'run'")
    groups: dict[tuple[str, str, str], list[dict[str, Any]]] = {}
    for result in results:
        if result["collection"] in manifest["collections"]:
            group = (result["collection"], result["variant"], result["configuration"])
            groups.setdefault(group, []).append(result)
    summaries = {group: summarize(found) for group, found in groups.items()}
    variants = [variant["name"] for variant in definition.variants]
    configurations = list(definition.planminer.configurations)

    lines = [
        f"# Banco de pruebas: {definition.name}",
        "",
        definition.description + ".",
        "",
        f"PDDLDomains {manifest['pddldomains'][:7]}, PyPlanner {manifest['pyplanner']}; "
        f"{definition.problems} problemas por dominio, validación cruzada con "
        f"{definition.folds} particiones (semilla {definition.seed}).",
        "",
        "Precisión (P), recall (R) y F del dominio aprendido frente al de referencia, en modo "
        "tolerante: media de las particiones y, entre paréntesis, su desviación típica. Validez: "
        "planes válidos con el dominio aprendido entre los problemas de prueba que resuelve la "
        "referencia; i inválidos (VAL los rechaza), u sin solución, l límite del planificador, "
        "e error (PyPlanner rechaza el dominio), f sin dominio (PlanMiner ha fallado).",
        "",
        "## Resumen",
        "",
        "Media de las colecciones de cada clase.",
        "",
        "| Clase | Variante | Configuración | P | R | F | Validez |",
        "|---|---|---|---|---|---|---|",
    ]
    classes = {
        "strips": [c for c, e in manifest["collections"].items() if e["class"] == "strips"],
        "numeric": [c for c, e in manifest["collections"].items() if e["class"] == "numeric"],
    }
    for label, members in classes.items():
        for variant in variants:
            for configuration in configurations:
                found = [
                    summaries[(c, variant, configuration)]
                    for c in members
                    if (c, variant, configuration) in summaries
                ]
                if not found:
                    continue
                cells = [
                    f"{_mean([s['tolerant']['total'][m]['mean'] for s in found]):.3f}"
                    for m in ("precision", "recall", "f_score")
                ]
                rates = [s["validity"] for s in found if s["validity"] is not None]
                validity = f"{100 * _mean(rates):.0f} %" if rates else "-"
                lines.append(
                    f"| {label} ({len(found)}) | {variant} | {configuration} | "
                    + " | ".join(cells)
                    + f" | {validity} |"
                )

    for variant in variants:
        for configuration in configurations:
            lines += [
                "",
                f"## {variant}, {configuration}",
                "",
                "| Colección | P | R | F | F exacto | Pre F | Efe F | Validez |",
                "|---|---|---|---|---|---|---|---|",
            ]
            for collection in sorted(manifest["collections"]):
                summary = summaries.get((collection, variant, configuration))
                if summary is None:
                    continue
                tolerant = summary["tolerant"]
                failures = summary["learning_failures"]
                note = f" ({failures} sin dominio)" if failures else ""
                lines.append(
                    f"| {collection}{note} | {_cell(tolerant['total']['precision'])} | "
                    f"{_cell(tolerant['total']['recall'])} | "
                    f"{_cell(tolerant['total']['f_score'])} | "
                    f"{summary['exact']['total']['f_score']['mean']:.3f} | "
                    f"{tolerant['precondition']['f_score']['mean']:.3f} | "
                    f"{tolerant['effect']['f_score']['mean']:.3f} | "
                    f"{validity_text(summary['outcomes'])} |"
                )

    if manifest["excluded"] or manifest["failed"]:
        lines += ["", "## Fuera del banco", ""]
        for identifier, reason in sorted(manifest["excluded"].items()):
            lines.append(f"- {identifier}: {reason}.")
        for identifier, error in sorted(manifest["failed"].items()):
            lines.append(f"- {identifier}: no se ha podido generar ({error.splitlines()[0]}).")
    text = "\n".join(lines) + "\n"
    (output / "report.md").write_text(text)
    (output / "report.json").write_text(
        json.dumps(
            [
                {"collection": c, "variant": v, "configuration": g} | summary
                for (c, v, g), summary in sorted(summaries.items())
            ],
            ensure_ascii=False,
            indent=2,
        )
        + "\n"
    )
    print(text)
    return 0


# --- La línea de comandos --------------------------------------------------------------------


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="El banco de pruebas de PlanMiner: genera los datos, hace la validación "
        "cruzada y escribe el informe."
    )
    commands = parser.add_subparsers(dest="command", required=True)
    for name, help_text in (
        ("generate", "genera los problemas, los planes y las trazas"),
        ("run", "la validación cruzada: aprende, compara y planifica"),
        ("report", "las tablas de los resultados"),
    ):
        command = commands.add_parser(name, help=help_text)
        command.add_argument(
            "benchmark",
            nargs="?",
            default="default",
            help="nombre en benchmarks/ o ruta a un .toml (por defecto, default)",
        )
        command.add_argument(
            "--output",
            type=Path,
            help="directorio de salida (por defecto, build/benchmark/<banco>)",
        )
        if name != "report":
            command.add_argument(
                "--jobs", type=int, default=3, help="trabajos en paralelo (por defecto, 3)"
            )
            command.add_argument(
                "--collections",
                nargs="+",
                default=[],
                metavar="PATRÓN",
                help="solo estas colecciones (admite comodines: 'ipc-2011/*')",
            )
            command.add_argument(
                "--validate",
                type=Path,
                default=ROOT / "tools" / "bin" / "Validate",
                help="ejecutable Validate de VAL (por defecto, tools/bin/Validate)",
            )
    commands.choices["generate"].add_argument(
        "--update-lock", action="store_true", help="guarda las huellas nuevas en el .lock.json"
    )
    commands.choices["run"].add_argument(
        "--planminer",
        type=Path,
        default=ROOT / "build" / "release" / "bin" / "planminer",
        help="ejecutable de PlanMiner (por defecto, build/release/bin/planminer)",
    )
    commands.choices["run"].add_argument(
        "--force", action="store_true", help="repite los aprendizajes que ya tienen resultado"
    )
    args = parser.parse_args(argv)

    try:
        definition = load_definition(args.benchmark)
        output = args.output or ROOT / "build" / "benchmark" / definition.name
        if args.command == "report":
            return report(definition, output)
        if not args.validate.is_file():
            raise BenchmarkError(f"no está VAL en {args.validate} (scripts/build_val.sh)")
        if args.command == "generate":
            tools = Tools(args.validate)
            return generate(
                definition, output, tools, args.jobs, args.collections, args.update_lock
            )
        tools = Tools(args.validate, args.planminer)
        return run(definition, output, tools, args.jobs, args.collections, args.force)
    except BenchmarkError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
