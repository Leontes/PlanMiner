# PlanMiner

**English** · [Español](README.es.md)

PlanMiner learns PDDL planning domains from plan traces, even incomplete ones. It also
discovers the logical and arithmetic expressions in preconditions and effects.

## Status

The project is being **rewritten from scratch in C++20**, with the algorithm ported step by
step. The original implementation lives in [`Old/`](Old/) and acts as the test oracle: each
ported part is compared with the original.

For now only the build and test infrastructure is in place. The `planminer` command line
just reports its version.

## Requirements

- Linux.
- CMake 3.25 or later.
- A C++20 compiler. Tested with GCC 13 and Clang 18.
- GoogleTest 1.14 or later. If it is not installed (`libgtest-dev`), CMake downloads it.

The planner tests also need:

- Python 3.12 or later, with [PyPlanner](https://github.com/Leontes/PyPlanner)
  (`scripts/setup_python.sh`).
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
| `planner` | learned domains: PyPlanner plans with them and VAL validates the plans against the reference domain (`tests/planner/evaluate_domain.py`) |

The planner tests are registered only when PyPlanner and VAL are available:

```bash
scripts/setup_python.sh   # .venv with PyPlanner
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # reconfigure so CMake finds them
```

To regenerate the oracle's reference domains:

```bash
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.legacy
```

## Layout

| Path | Contents |
|------|----------|
| `include/planminer/`, `src/` | the `planminer::core` library |
| `apps/planminer/` | the command line |
| `tests/unit/` | unit tests |
| `tests/oracle/` | comparisons with `Old/` and reference domains (`golden/`) |
| `tests/planner/` | domain validation with PyPlanner and VAL |
| `tests/data/` | reference domains and problems |
| `cmake/` | CMake modules: warnings, sanitizers, dependencies, `Old/` and the planner |
| `scripts/` | PyPlanner and VAL setup |
| `Old/` | the original implementation, with a minimal patch so it builds |

## Conventions

- Identifiers are in English; comments, documentation and messages are in Spanish.
- C++ formatting follows `.clang-format` (clang-format 18). Python is checked with `ruff`
  and `mypy --strict`.
- Compiler warnings are not tolerated in new code: CI builds with `-Werror`.

## License

[GPL-3.0](LICENSE).
