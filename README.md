# PlanMiner

**English** · [Español](README.es.md)

Learning of PDDL planning domains from plan traces, even incomplete or noisy ones. PlanMiner
discovers the preconditions and effects of every action, including the arithmetic expressions
of numeric fluents.

This is a C++20 rewrite of the original PlanMiner, with improvements over the published
algorithm ([Segura-Muros, Pérez and Fernández-Olivares,
2021](https://doi.org/10.1007/s10489-021-02232-6)). The original algorithm is still available
(`--original`), and every improvement can be switched off and measured against it.

## What it learns

| Language | What it includes | Status |
|----------|------------------|--------|
| STRIPS | preconditions, add and delete effects, with types and action costs | yes |
| PDDL 2.1 without time | numeric fluents: preconditions with comparisons and effects with arithmetic expressions | yes |
| ADL | quantifiers, conditional effects, disjunctions and domain constants | no |
| Temporal PDDL 2.1 | durative actions | no |

With complete traces, or with 30 % of the atoms and values unknown, the improved configuration
gives valid plans in every test problem of 14 of the 15 domains of the benchmark (the other one
uses domain constants). With 5 % noise, not yet: see
[Evaluation](docs/en/evaluation.md#results).

## Features

- **Input:** traces in the [PDDLDomains](https://github.com/Leontes/PDDLDomains) format
  (`pddldomains trace`): the states a plan goes through when executed, in JSON, with gaps,
  missing states and noise.
- **Logical preconditions and effects:** rules that describe the state before each action and the
  state after it, learned with [INSLV](https://github.com/Leontes/INSLV-PlanMiner) (fuzzy rules,
  with the NSLV genetic algorithm) and fused.
- **Numeric effects:** symbolic regression, an A* search for the arithmetic expression of each
  change: `(decrease (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))`.
- **Numeric preconditions:** comparisons between fluents and with the expressions of the
  effects: `(>= (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))`.
- **Types:** the hierarchy is inferred from the objects in the traces.
- **Two configurations:** the improved one (the default) and the original one. The improvements
  are described in [How PlanMiner learns](docs/en/algorithm.md).
- **Memory:** PlanMiner sets itself a memory limit, and its searches stop when they reach it
  instead of exhausting the machine's memory.
- **Evaluation:** precision, recall and F of a learned domain against the reference one, and a
  reproducible benchmark with cross-validation that plans with the learned domains and
  validates the plans with VAL.

## Installation

Requires Linux, CMake 3.25 or later, git and a C++20 compiler (tested with GCC 13 and Clang 18).

```bash
git clone --recursive https://github.com/Leontes/PlanMiner.git
cd PlanMiner
cmake --preset release
cmake --build --preset release      # the executable: build/release/bin/planminer
```

| Preset | Use |
|--------|-----|
| `release` | normal use and experiments |
| `debug` | development |
| `asan` | Debug with AddressSanitizer and UBSan |
| `ci-gcc`, `ci-clang` | Release with warnings as errors, as in continuous integration |

Each preset builds in `build/<preset>`. [INSLV](https://github.com/Leontes/INSLV-PlanMiner) is
downloaded when configuring (FetchContent), pinned to a version (`v1.0.0`). The
`vendor/PDDLDomains` submodule (the domain catalog), pinned to its version `v1.0`, is only
needed for the tests and the evaluation.

## Language

The command-line help, the messages and the errors are in Spanish. The identifiers (options,
configuration keys, PDDL names) are in English. The documentation is in Spanish and English.

## Usage

### From the terminal

```bash
planminer -o domain.pddl traces/*.json          # improved configuration
planminer --original -o domain.pddl traces/*.json
planminer -n blocks -m 2048 traces/*.json        # to standard output, with at most 2 GB
```

PlanMiner reads the traces of one domain and writes the learned domain to the file (or to
standard output, without `-o`).

| Option | Use |
|--------|-----|
| `-o`, `--output <file>` | the domain file |
| `-n`, `--name <name>` | the domain name; by default, the one in the traces (some planners require it to match the problems' one) |
| `-m`, `--memory-limit <MB>` | the memory limit; by default, 80 % of the available memory |
| `--original` | learns with the original PlanMiner algorithm, without improvements |
| `-h`, `--help`, `--version` | help and version |

Exit codes:

- 0: domain learned;
- 1: the traces could not be read or the domain could not be written;
- 2: invalid arguments.

With `--original` and a numeric domain, the learned domain declares the functions but not the
`:numeric-fluents` requirement, as the original PlanMiner did, and some planners reject it.

### As a library

Each part of the algorithm is a static CMake library (see [Structure](#structure)). `pddl`
learns the domain end to end:

```cpp
#include "pddl/domain.hpp"
#include "plantraces/trace.hpp"

std::vector<plantraces::Trace> traces = {plantraces::read_trace("p01.json"),
                                         plantraces::read_trace("p02.json")};
pddl::LearnedDomain domain = pddl::learn(traces);  // or pddl::learn(traces, pddl::Options::original())
std::string text = pddl::write("blocks", domain);
```

- **Errors:** traces that cannot be read throw `plantraces::TraceError`.
- **Options:** `pddl::Options` gathers the options of every part (`Options{}` is the improved
  configuration and `Options::original()`, the original one); each improvement is a field that
  can be switched off on its own.
- **Reproducibility:** learning does not use the global `rand()`. The discretization follows the
  sequence of INSLV's generator, which is that of glibc's `rand()`, in the same order as the
  original: the original configuration gives the same examples and rules.
- **Memory:** `resources::limit_process` sets a hard limit on the process, as the command line
  does.

## Structure

| Path | Contents |
|------|----------|
| `apps/planminer/` | the command line |
| `libs/plantraces/` | trace reading, the applications of each action, atoms over its parameters, the type hierarchy and noise (its estimate and atom smoothing) |
| `libs/datasets/` | the example table of each action, numeric effects (with the regression), numeric relations and discretization |
| `libs/regression/` | symbolic regression: arithmetic expressions and their A* search |
| `libs/rules/` | the INSLV examples, the interpretation of its rules and their fusion into precondition and effect |
| `libs/pddl/` | end-to-end learning, precondition simplification and PDDL writing |
| `libs/resources/` | memory limits |
| `vendor/PDDLDomains/` | submodule: the domain catalog of the tests and the benchmark (`v1.0`) |
| `scripts/` | domain comparison, the benchmark, the test traces and environment setup |
| `benchmarks/` | the benchmark definitions and their fingerprints |
| `tests/` | unit, planner, script and oracle tests; `data/traces/`: the test traces |
| `cmake/` | CMake modules: warnings, sanitizers, dependencies (INSLV among them) and test tools |
| `docs/es/`, `docs/en/` | documentation, in Spanish and English |

`build/` (builds), `tools/` (VAL built locally), `.venv/` (the Python environment), `Old/` (the
original code, the tests' oracle) and `privado/` (private documents) are not in git.

## Documentation

- [How PlanMiner learns](docs/en/algorithm.md): the parts of the algorithm and the improvements
  of the improved configuration.
- [Evaluation](docs/en/evaluation.md): domain comparison, the benchmark and its results.
- [Tests](docs/en/tests.md): labels, test traces, the oracle and memory limits.
- [Licenses](LICENSES.md): PlanMiner's, INSLV's and the test domains'.

## Development

```bash
git clone --recursive https://github.com/Leontes/PlanMiner.git   # with the catalog
git submodule update --init vendor/PDDLDomains                    # or, in an existing clone
scripts/setup_python.sh     # .venv with PyPlanner and pddldomains
scripts/build_val.sh        # builds VAL into tools/bin/Validate (needs git, cmake and g++)
cmake --workflow --preset debug                                   # configure, build and test
git ls-files -- '*.cpp' '*.hpp' | xargs clang-format --dry-run --Werror
ruff check . && ruff format --check . && mypy --strict tests/scripts scripts
```

- **Tests:** by CTest label (`ctest --preset debug -L unit`): `unit`, `planner` (with
  PyPlanner, VAL and PDDLDomains) and `oracle` (only with `Old/`, locally). See
  [Tests](docs/en/tests.md).
- **Local INSLV:** to try a local copy instead of the version pinned in `cmake/Inslv.cmake`,
  `cmake --preset debug -DFETCHCONTENT_SOURCE_DIR_INSLV=<directory>`.
- **Test dependencies:** GoogleTest 1.14 or later (if it is not installed, CMake downloads it);
  for the planner tests, Python 3.12 or later with
  [PyPlanner](https://github.com/Leontes/PyPlanner) and pddldomains (`scripts/setup_python.sh`)
  and [VAL](https://github.com/KCL-Planning/VAL) (`scripts/build_val.sh`). After installing
  them, configure again.
- **Continuous integration:** builds and tests with GCC and Clang, with the sanitizers and with
  the planner tests, and checks the formatting (clang-format 18) and the scripts (ruff and
  mypy).

## Citing

If you use PlanMiner in a publication, please cite the paper:

> José Á. Segura-Muros, Raúl Pérez and Juan Fernández-Olivares. Discovering relational and
> numerical expressions from plan traces for learning action models. *Applied Intelligence*,
> 51(11):7973–7989, 2021. [doi:10.1007/s10489-021-02232-6](https://doi.org/10.1007/s10489-021-02232-6)

```bibtex
@article{seguramuros2021planminer,
  author  = {Segura-Muros, Jos{\'e} {\'A}. and P{\'e}rez, Ra{\'u}l and Fern{\'a}ndez-Olivares, Juan},
  title   = {Discovering relational and numerical expressions from plan traces for learning action models},
  journal = {Applied Intelligence},
  volume  = {51},
  number  = {11},
  pages   = {7973--7989},
  year    = {2021},
  doi     = {10.1007/s10489-021-02232-6}
}
```

## License

The code and the documentation are distributed under the [MIT license](LICENSE) (© 2026 José
Segura). [INSLV](https://github.com/Leontes/INSLV-PlanMiner), which is downloaded when configuring
and is based on NSLV by Raúl Pérez, is under the GPL-3.0, and the `planminer` executable, which
links it, is subject to it. The domains and problems of the PDDLDomains catalog (submodule) have
their own terms. See [LICENSES.md](LICENSES.md).
