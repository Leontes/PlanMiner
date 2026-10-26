# PlanMiner

**English** · [Español](README.es.md)

PlanMiner learns PDDL planning domains from plan traces, even incomplete ones. It also
discovers the logical and arithmetic expressions in preconditions and effects.

> **In development.** PlanMiner is being rewritten in C++20, with improvements over the original
> algorithm that are added step by step. The original algorithm is still available
> (`--original`).

## Traces

PlanMiner learns from traces in the [PDDLDomains](https://github.com/Leontes/PDDLDomains)
format (`pddldomains trace`): the states a plan goes through, with gaps, missing states and
noise, and its actions. The format is described in
[`vendor/PDDLDomains/docs/en/traces.md`](https://github.com/Leontes/PDDLDomains/blob/main/docs/en/traces.md).

## Requirements

- Linux.
- CMake 3.25 or later.
- A C++20 compiler. Tested with GCC 13 and Clang 18.
- GoogleTest 1.14 or later, for the tests. If it is not installed (`libgtest-dev`), CMake
  downloads it.

The planner tests also need:

- The `vendor/PDDLDomains` submodule (`git clone --recursive`, or
  `git submodule update --init` in an existing clone).
- Python 3.12 or later, with [PyPlanner](https://github.com/Leontes/PyPlanner) and
  `pddldomains` (`scripts/setup_python.sh`).
- [VAL](https://github.com/KCL-Planning/VAL) (`scripts/build_val.sh`, needs git, cmake and
  g++).

## Build and test

```bash
cmake --workflow --preset debug     # configure, build and test

# or step by step
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

| Preset | Use |
|--------|-----|
| `debug` | development |
| `release` | normal use and experiments |
| `asan` | Debug with AddressSanitizer and UBSan; its test step runs only the unit tests |
| `ci-gcc`, `ci-clang` | Release with warnings as errors |

Each preset builds in `build/<preset>`, and the executable is `build/<preset>/bin/planminer`.

Tests are grouped by CTest label (`ctest --preset debug -L <label>`):

| Label | What it checks |
|-------|----------------|
| `unit` | the libraries, the command line and the scripts |
| `planner` | learned domains: PyPlanner plans with them and VAL validates the plans against the reference domain |
| `inslv` | the fuzzy rule learning library (`vendor/inslv`) |

The planner tests are registered only when PyPlanner, pddldomains and VAL are available:

```bash
scripts/setup_python.sh   # .venv with PyPlanner and pddldomains
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # reconfigure so CMake finds them
```

## Usage

```bash
planminer -o domain.pddl traces/*.json
```

PlanMiner reads traces of a single domain and writes the learned domain to the file (or to
standard output, without `-o`).

| Option | Use |
|--------|-----|
| `-o`, `--output <file>` | the domain file |
| `-n`, `--name <name>` | the domain name; by default, the traces' (some planners require it to match the problems') |
| `-m`, `--memory-limit <MB>` | the memory limit; by default, 80 % of the available memory |
| `--original` | learns with the algorithm of the original PlanMiner, without improvements |
| `-h`, `--help`, `--version` | help and version |

It exits with 0 on success, 1 if it cannot read the traces or write the domain, and 2 if the
arguments are not valid.

With `--original` and a numeric domain, the learned domain declares its functions but not the
`:numeric-fluents` requirement, as the original PlanMiner did, and some planners reject it.

## Evaluation

`scripts/compare_domains.py` compares a learned domain with the reference one (it only needs
Python 3.12 or later):

```bash
scripts/compare_domains.py domain.pddl reference.pddl [--tolerant [--symmetric NAME]...] \
    [--details] [--json]
```

Each element of the preconditions and effects of the actions (an atom, its negation, a numeric
comparison or a numeric effect) counts as:

- a true positive if it is in both domains;
- a false positive if it is only in the learned one (it is extra);
- a false negative if it is only in the reference one (it is missing).

Adding up all the actions, it reports the precision, recall and F-score of the preconditions,
of the effects and of both. `--details` lists the extra and missing elements and the half hits
of each action, and `--json` writes the result as JSON.

Actions are matched by name, and their parameters by position; types are not compared. Before
comparing, elements are normalized (case, number format, `>` as `<` with the sides swapped,
the order of the operands of `=`, `+` and `*`), but no other equivalence is recognized:
`(< a b)` and `(<= a b)` are different elements.

With `--tolerant`, an extra element and a missing one that are almost equal count as half a
hit (half a true positive, half a false positive and half a false negative) instead of a false
positive and a false negative. They are almost equal if they only differ in `<` versus `<=`, or
in the order of the arguments of a symmetric symbol: a function or predicate of two arguments
whose value does not depend on their order, such as the distance between two cities, given
with `--symmetric` (once per symbol).

It exits with 0 if the domains could be compared and 2 if the arguments or the domains are not
valid.

## Memory

PlanMiner limits the memory it uses so it cannot exhaust the machine's (on WSL, exhausting it
freezes all of WSL):

- The command line sets a limit of 80 % of the available memory at startup, or the one given
  with `-m`. When a search reaches it, the search stops and reports it instead of allocating
  more.
- Every test executable is limited to 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, no limit). With
  `ctest` in parallel (4 jobs), the worst case is 8 GB.
- The tests always run PyPlanner with a memory limit (`-m`).

## License

[GPL-3.0](LICENSE).
