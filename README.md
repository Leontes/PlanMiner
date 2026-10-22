# PlanMiner

**English** · [Español](README.es.md)

PlanMiner learns PDDL planning domains from plan traces, even incomplete ones. It also
discovers the logical and arithmetic expressions in preconditions and effects.

## Status

The project is being **rewritten from scratch in C++20**, with the algorithm ported step by
step. The original implementation lives in [`Old/`](Old/) and acts as the test oracle: each
ported part is compared with the original.

For now the build and test infrastructure and the trace reader are in place. The `planminer`
command line just reports its version.

## Traces

PlanMiner learns from traces in the [PDDLDomains](https://github.com/Leontes/PDDLDomains)
format (`pddldomains trace`): the states a plan goes through, with gaps, missing states and
noise, and its actions. The format is described in
[`vendor/PDDLDomains/docs/en/traces.md`](https://github.com/Leontes/PDDLDomains/blob/main/docs/en/traces.md).

## Requirements

- Linux.
- CMake 3.25 or later.
- A C++20 compiler. Tested with GCC 13 and Clang 18.
- GoogleTest 1.14 or later. If it is not installed (`libgtest-dev`), CMake downloads it.

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
| `release` | experiments |
| `asan` | Debug with AddressSanitizer and UBSan; its test step runs only the unit tests |
| `ci-gcc`, `ci-clang` | CI: Release with warnings as errors |

Each preset builds in `build/<preset>`.

### Tests

Tests are grouped by CTest label (`ctest --preset debug -L <label>`):

| Label | What it checks |
|-------|----------------|
| `unit` | the new code |
| `oracle` | the comparison with `Old/`. Some tests call the original functions through the `planminer::legacy` target. Others compare the domain the original learns from the `Old/Test` traces with the one stored in `tests/oracle/golden/` |
| `planner` | learned domains: PyPlanner plans with them and VAL validates the plans against the reference domain (`tests/planner/evaluate_domain.py`). Also the test traces, which must regenerate identically |
| `inslv` | the INSLV library in `vendor/inslv`, including its equivalence with `Old/` |

The planner tests are registered only when PyPlanner, pddldomains and VAL are available:

```bash
scripts/setup_python.sh   # .venv with PyPlanner and pddldomains
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # reconfigure so CMake finds them
```

To regenerate the oracle's reference outputs (the domains in `tests/oracle/golden/` and the
INSLV cases in `vendor/inslv/tests/cases/`):

```bash
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R "oracle.legacy|oracle.inslv_cases"
```

### Test traces

`tests/data/traces/` holds traces of problems from the PDDLDomains catalog, with plans from
PyPlanner. `tests/data/traces/dataset.toml` lists the problems and the variants (complete,
closed world, with gaps, with noise and with missing states). The plans are stored so the
traces can be regenerated without planning again:

```bash
.venv/bin/python scripts/generate_traces.py            # missing plans and all traces
.venv/bin/python scripts/generate_traces.py --replan   # plans too
.venv/bin/python scripts/generate_traces.py --check    # check they regenerate identically
```

## Layout

| Path | Contents |
|------|----------|
| `include/planminer/`, `src/` | the `planminer::core` library |
| `apps/planminer/` | the command line |
| `tests/unit/` | unit tests |
| `tests/oracle/` | comparisons with `Old/` and reference domains (`golden/`) |
| `tests/planner/` | domain validation with PyPlanner and VAL |
| `tests/data/` | reference domains and problems, and the test traces (`traces/`) |
| `cmake/` | CMake modules: warnings, sanitizers, dependencies, `Old/` and the planner |
| `scripts/` | PyPlanner, pddldomains and VAL setup, and test trace generation |
| `vendor/inslv/` | INSLV, the fuzzy rule learner (see its [README](vendor/inslv/README.md), in Spanish) |
| `vendor/PDDLDomains/` | submodule: the domain catalog and the trace tool |
| `Old/` | the original implementation, with a minimal patch so it builds |

## INSLV

Fuzzy rule learning (INSLV) is out of scope for the rewrite. It lives in
[`vendor/inslv`](vendor/inslv/) as an external dependency with its own interface
(`inslv::learn`) until it gets its own repository.

## Conventions

- Identifiers are in English; comments, documentation and messages are in Spanish.
- C++ formatting follows `.clang-format` (clang-format 18). Python is checked with `ruff`
  and `mypy --strict`.
- Compiler warnings are not tolerated in new code: CI builds with `-Werror`.

## License

[GPL-3.0](LICENSE).
