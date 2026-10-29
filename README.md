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
| `planner` | with PyPlanner, VAL and PDDLDomains: the harness that validates learned domains, the test traces and the minimal benchmark |
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

### Comparing a domain

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

### Benchmark

`scripts/benchmark.py` evaluates PlanMiner on the domains of the PDDLDomains catalog that it can
learn: STRIPS (with types and action costs) and PDDL 2.1 without durative actions (numeric
fluents). It needs the Python environment, VAL and PlanMiner built with the `release` preset:

```bash
.venv/bin/python scripts/benchmark.py generate   # problems, plans and traces
.venv/bin/python scripts/benchmark.py run        # cross-validation
.venv/bin/python scripts/benchmark.py report     # tables in build/benchmark/default/report.md
```

The benchmark is defined in [`benchmarks/default.toml`](benchmarks/default.toml), which explains
each setting:

- **Domains.** The collections that the selection asks PDDLDomains for. Those with a domain per
  problem are left out, because PlanMiner learns a single domain.
- **Problems.** 90 per domain, each with its plan. In classical domains, they are random walks
  (the `random-walk` generator of PDDLDomains) from the 6 smallest problems of the collection,
  and the walk is the plan. In numeric domains, the collection's generator gives the problems
  and PyPlanner the plans; the problems it does not solve are discarded.
- **Coverage.** The walks give 3 candidates per problem, and the chosen ones cover the domain:
  every action applied and every effect changing the state in at least 6 problems, or in all
  the candidates that show it, if there are fewer. What no problem shows (an action that cannot
  be applied in any problem of the collection) does not count in the metrics, because it cannot
  be learned; the report lists it, along with anything missing from the training data of a
  fold.
- **Traces.** One per plan and variant: complete, with 30 % of the atoms and values unknown
  (`missing30`) and with 5 % noise (`noise5`).
- **Cross-validation.** The problems of each domain are split into 3 folds. For each fold,
  variant and configuration (`--original` or not), PlanMiner learns the domain from the traces
  of the other folds. The learned domain is compared with the reference one (precision, recall
  and F in tolerant mode, and also exact). PyPlanner then plans the fold's problems with it,
  and VAL validates the plans against the reference domain. Validity only counts the problems
  that the reference domain solves.
- **Report.** For each domain, variant and configuration: the mean and standard deviation over
  the folds of precision, recall and F, and the valid plans. The plans that are not valid are
  broken down by cause: invalid, unsolvable, planner limit, error (PyPlanner rejects the
  domain) and no domain (PlanMiner failed).

Everything is reproducible:

- The seeds are fixed.
- `generate` checks the hashes of what it generates against `benchmarks/default.lock.json`, and
  exits with 1 if they differ (`--update-lock` saves the new ones).
- `run` saves the result of every learning and continues where it left off (`--force` repeats
  them).

Other options:

| Option | Use |
|--------|-----|
| `--jobs N` | parallel jobs (3 by default); they must fit in 80 % of the available memory |
| `--collections PATTERN...` | only these collections (`'ipc-2011/*'`) |
| `--configurations NAME...` | with `run`, only these PlanMiner configurations (`original`, `improved`) |
| `--output DIR` | the output directory (by default, `build/benchmark/<benchmark>`) |
| `--planminer FILE`, `--validate FILE` | the PlanMiner and VAL executables |

Another benchmark is another TOML: `scripts/benchmark.py run my.toml`.
[`benchmarks/smoke.toml`](benchmarks/smoke.toml) is the minimal one of the `planner` tests.

## Memory

PlanMiner limits the memory it uses so it cannot exhaust the machine's (on WSL, exhausting it
freezes all of WSL):

- The command line sets a limit of 80 % of the available memory at startup, or the one given
  with `-m`. When a search reaches it, the search stops and reports it instead of allocating
  more.
- Every test executable is limited to 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, no limit). With
  `ctest` in parallel (4 jobs), the worst case is 8 GB.
- The tests always run PyPlanner with a memory limit (`-m`).
- The benchmark runs PlanMiner and PyPlanner with the limits of its definition (2560 MB and
  1024 MB by default), and refuses to start if its jobs do not fit in 80 % of the available
  memory.

## License

[GPL-3.0](LICENSE).
