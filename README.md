# PlanMiner

**English** · [Español](README.es.md)

PlanMiner learns PDDL planning domains from plan traces, even incomplete ones. It also
discovers the logical and arithmetic expressions in preconditions and effects.

> **In development.** PlanMiner is being rewritten in C++20. The command line does not learn
> domains yet: for now it only reports its version.

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

Each preset builds in `build/<preset>`. The executable is `build/<preset>/bin/planminer`:

```bash
build/release/bin/planminer --version
```

Tests are grouped by CTest label (`ctest --preset debug -L <label>`):

| Label | What it checks |
|-------|----------------|
| `unit` | the libraries and the command line |
| `planner` | learned domains: PyPlanner plans with them and VAL validates the plans against the reference domain |
| `inslv` | the fuzzy rule learning library (`vendor/inslv`) |

The planner tests are registered only when PyPlanner, pddldomains and VAL are available:

```bash
scripts/setup_python.sh   # .venv with PyPlanner and pddldomains
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # reconfigure so CMake finds them
```

## Memory

PlanMiner limits the memory it uses so it cannot exhaust the machine's (on WSL, exhausting it
freezes all of WSL):

- The command line sets a limit of 80 % of the available memory at startup. When a search
  reaches it, the search stops and reports it instead of allocating more.
- Every test executable is limited to 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, no limit). With
  `ctest` in parallel (4 jobs), the worst case is 8 GB.
- The tests always run PyPlanner with a memory limit (`-m`).

## License

[GPL-3.0](LICENSE).
