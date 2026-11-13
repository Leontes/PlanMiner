# Tests

**English** · [Español](../es/pruebas.md)

Tests are grouped by CTest label:

```bash
ctest --preset debug                  # every registered test
ctest --preset debug -L unit          # only the unit tests
ctest --preset debug -R 'unit.pddl.*' # by name
```

| Label | What it checks | Needs |
|-------|----------------|-------|
| `unit` | the libraries, the command line and the Python scripts | GoogleTest (if it is not installed, CMake downloads it) |
| `planner` | the harness that validates learned domains, that the test traces regenerate identically and the minimal benchmark, end to end | PyPlanner, pddldomains, VAL and the `vendor/PDDLDomains` submodule |
| `oracle` | the original configuration against the original PlanMiner (`Old/`), function by function and end to end | `Old/`, locally only |

INSLV's tests run in [its repository](https://github.com/Leontes/INSLV-PlanMiner): when it is
downloaded, they are not built.

## Planner tests

They validate learned domains by planning with them: PyPlanner looks for a plan with the learned
domain and VAL validates it with the reference one (`scripts/evaluate_domain.py`). They are only
registered if the tools are there:

```bash
scripts/setup_python.sh   # .venv with PyPlanner (tests/planner/requirements.txt) and pddldomains
scripts/build_val.sh      # VAL in tools/bin/Validate
cmake --preset debug      # configure again so that it finds them
```

- **The harness:** the reference domain, evaluated against itself, gives valid plans, and a
  faulty one, plans that VAL rejects. Validity is decided by VAL's text ("Plan valid"): its exit
  code is 0 also for some invalid plans.
- **The test traces** (`planner.traces_reproducible`): they regenerate identically from their
  plans.
- **The minimal benchmark** (`planner.benchmark.*`): it generates the data of
  [`benchmarks/smoke.toml`](../../benchmarks/smoke.toml), which must match its fingerprints,
  runs the cross-validation with the freshly built PlanMiner and writes the report.

## Test traces

[`tests/data/traces`](../../tests/data/traces) holds the traces of the unit and oracle tests:
blocksworld, counters and zenotravel, from the PDDLDomains catalog, in several variants
(complete, closed world, with gaps, with missing states and with noise). Zenotravel also has small
problems from its generator (stored with the traces): fly-fast is rare in the plans of the catalog
problems, and with gaps its effects could be left without data.
[`dataset.toml`](../../tests/data/traces/dataset.toml) defines them, and
`scripts/generate_traces.py` generates them (with `--check`, it checks that they regenerate
identically without writing anything). The plans come from PyPlanner (`gbfs-ff-pref` on one
core, which is deterministic).

## The oracle

During the rewrite, each part was compared with the original code, which is still in the git
history but no longer in the repository (`Old/` is in `.gitignore`). The oracle tests are only
built if `Old/` exists (`PLANMINER_BUILD_LEGACY`), so continuous integration does not run them.
To get it back:

```bash
git archive 3823a14 Old | tar -x
```

They check that the original configuration gives the same results as the original: function by
function, end to end with the test traces (the learned domain, compared with the one saved in
`tests/oracle/golden/`) and INSLV's equivalence cases, which are in its repository
(`tests/datos/planminer`). To regenerate what is saved:

```bash
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.legacy
# INSLV's cases, in a local copy (-DFETCHCONTENT_SOURCE_DIR_INSLV=<directory>):
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.inslv_cases
```

## Memory

- Each test executable is limited to 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, no limit). With
  `ctest` in parallel (4 jobs), the worst case is 8 GB.
- The tests always run PyPlanner with a memory limit (`-m`).
- With AddressSanitizer (the `asan` preset), the process memory limit does not work:
  `ASAN_OPTIONS=hard_rss_limit_mb` is used instead.

## Continuous integration

[`.github/workflows/ci.yml`](../../.github/workflows/ci.yml):

- build and tests with GCC and with Clang (warnings as errors);
- AddressSanitizer and UBSan;
- the planner tests, with VAL built (cached) and the Python environment;
- the C++ formatting (clang-format 18) and the Python scripts (ruff and `mypy --strict`).
