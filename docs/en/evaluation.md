# Evaluation

**English** · [Español](../es/evaluacion.md)

Two tools: `scripts/compare_domains.py` compares a learned domain with the reference one, and
`scripts/benchmark.py` evaluates PlanMiner with a reproducible benchmark.

## Comparing a domain

```bash
scripts/compare_domains.py domain.pddl reference.pddl [--tolerant [--symmetric NAME]...] \
    [--details] [--json]
```

It only needs Python 3.12 or later. Each element of the preconditions and effects of the
actions (an atom, its negation, a numeric comparison or a numeric effect) counts as:

- a true positive if it is in both domains;
- a false positive if it is only in the learned one (extra);
- a false negative if it is only in the reference one (missing).

Adding up all the actions, it gives the precision, recall and F-score of the preconditions, of
the effects and of everything together. `--details` lists the extra and missing elements and the
near matches of each action, and `--json` writes the result in JSON.

Actions are matched by name, and their parameters by position; types are not compared. Before
comparing, the elements are normalized (case, number format, `>` as `<` with the sides
swapped, the order of the operands of `=`, `+` and `*`), but no other equivalences are
recognized: `(< a b)` and `(<= a b)` are different elements.

With `--tolerant`, an extra element and a missing one that are almost equal count as a near
match (half a true positive, half a false positive and half a false negative) instead of a
false positive and a false negative. They are almost equal if they only differ in `<` versus
`<=`, or in the order of the arguments of a symmetric symbol: a two-argument function or
predicate whose value does not depend on their order, such as the distance between two cities,
given with `--symmetric` (once per symbol).

It exits with 0 if it could compare the domains and with 2 if the arguments or the domains are
invalid.

## The benchmark

`scripts/benchmark.py` evaluates PlanMiner with the domains of the PDDLDomains catalog that it
can learn: STRIPS (with types and action costs) and PDDL 2.1 without durative actions (numeric
fluents). It needs the Python environment, VAL and PlanMiner built with the `release` preset:

```bash
.venv/bin/python scripts/benchmark.py generate   # problems, plans and traces
.venv/bin/python scripts/benchmark.py run        # cross-validation
.venv/bin/python scripts/benchmark.py report     # tables in build/benchmark/default/report.md
```

The benchmark is defined in [`benchmarks/default.toml`](../../benchmarks/default.toml), which
explains each setting:

- **Domains.** The collections the selection asks PDDLDomains for. Those with one domain per
  problem are left out, because PlanMiner learns only one.
- **Problems.** 90 per domain, each with its plan. In the classical ones they are random walks
  (the `random-walk` generator of PDDLDomains) from the 6 smallest problems of the collection,
  and the walk is the plan. In the numeric ones, the collection's generator gives the problems
  and PyPlanner the plans; the problems it does not solve are discarded.
- **Coverage.** The walks give 3 candidates per problem, and the chosen ones cover the domain:
  every action applied and every effect changing the state in at least 6 problems, or in all
  the candidates that show it, if there are fewer. What no problem shows (an action that cannot
  be applied in any problem of the collection) does not count in the metrics, because it cannot
  be learned; the report lists it, along with anything missing from the training data of a
  fold.
- **Traces.** One per plan and variant: complete (`complete`), with 30 % of the atoms and values
  unknown (`missing30`) and with 5 % noise (`noise5`).
- **Cross-validation.** The problems of each domain are split into 3 folds. With each fold,
  variant and configuration (`--original` or not), PlanMiner learns the domain with the traces
  of the other folds. The learned domain is compared with the reference one (precision, recall
  and F in tolerant mode, and also exact). Then PyPlanner plans with it the problems of the
  fold, and VAL validates the plans with the reference domain. Validity only counts the
  problems the reference domain solves.
- **Report.** For each domain, variant and configuration: the mean and standard deviation over
  the folds of precision, recall and F, and the valid plans. Plans that are not valid are broken
  down by cause: invalid (`i`, VAL rejects them), unsolvable (`u`), planner limit (`l`), error
  (`e`, PyPlanner rejects the domain) and no domain (`f`, PlanMiner failed).

Everything is reproducible:

- The seeds are fixed.
- `generate` checks the fingerprints of what it generates against those in
  `benchmarks/default.lock.json`, and exits with 1 if they do not match (`--update-lock` saves
  the new ones).
- `run` saves the result of each learning and resumes where it left off (`--force` repeats
  them).

Other options:

| Option | Use |
|--------|-----|
| `--jobs N` | parallel jobs (3 by default); they must fit in 80 % of the available memory |
| `--collections PATTERN...` | only these collections (`'ipc-2011/*'`) |
| `--configurations NAME...` | with `run`, only these PlanMiner configurations (`original`, `improved`) |
| `--output DIR` | the output directory (by default, `build/benchmark/<benchmark>`) |
| `--planminer FILE`, `--validate FILE` | the PlanMiner and VAL executables |

Another benchmark is another TOML: `scripts/benchmark.py run mine.toml`.
[`benchmarks/smoke.toml`](../../benchmarks/smoke.toml) is the minimal one of the `planner`
tests, and [`benchmarks/ruido.toml`](../../benchmarks/ruido.toml), the noise one: the same
domains, problems and plans as `default`, with complete traces and with 3, 5, 10 and 20 % noise,
without gaps (the complete and 5 % ones are those of `default`; `scripts/benchmark.py generate
ruido`, `run ruido`).

The benchmark runs PlanMiner and PyPlanner with the memory limits of its definition (2560 MB and
1024 MB by default), and does not start if its jobs do not fit in 80 % of the available memory.
PyPlanner's time limit (`-t`) only covers the search, so its whole run is stopped at twice the
limit.

## Results

The `default` benchmark with PlanMiner at commit `f69da15` (October 2026): 15 domains, 90
problems per domain and 3 folds. P, R and F in tolerant mode; validity is the percentage of
valid plans. INSLV 1.0 gives the same results: with it, the 270 learned domains are
byte-for-byte identical.

| Class | Variant | Original: F · validity | Improved: F · validity |
|-------|---------|-----------------------:|-----------------------:|
| STRIPS (13) | complete | 0.937 · 87 % | 0.958 · 92 % |
| STRIPS (13) | missing30 | 0.916 · 80 % | 0.958 · 92 % |
| STRIPS (13) | noise5 | 0.574 · 2 % | 0.422 · 1 % |
| numeric (2) | complete | 0.749 · 51 % | 0.820 · 100 % |
| numeric (2) | missing30 | 0.704 · 53 % | 0.815 · 100 % |
| numeric (2) | noise5 | 0.359 · 4 % | 0.522 · 51 % |

Per domain, with complete traces (F · valid plans):

| Collection | Original | Improved |
|------------|---------:|---------:|
| `ipc-1998/gripper-round-1-strips` | 0.977 · 27/90 | 1.000 · 90/90 |
| `ipc-2000/blocks-strips-untyped` | 1.000 · 90/90 | 1.000 · 90/90 |
| `ipc-2011/barman-sequential-satisficing` | 0.976 · 89/89 | 0.979 · 89/89 |
| `ipc-2011/elevator-sequential-satisficing` | 0.901 · 82/82 | 0.901 · 82/82 |
| `ipc-2011/floor-tile-sequential-satisficing` | 0.919 · 90/90 | 0.919 · 90/90 |
| `ipc-2011/no-mystery-sequential-satisficing` | 0.909 · 90/90 | 1.000 · 90/90 |
| `ipc-2011/parking-sequential-satisficing` | 0.947 · 90/90 | 0.947 · 90/90 |
| `ipc-2011/peg-solitaire-sequential-satisficing` | 0.952 · 90/90 | 0.984 · 90/90 |
| `ipc-2011/scanalyzer-3d-sequential-satisficing` | 0.979 · 89/89 | 0.979 · 89/89 |
| `ipc-2011/sokoban-sequential-satisficing` | 1.000 · 90/90 | 1.000 · 90/90 |
| `ipc-2011/transport-sequential-satisficing` | 0.965 · 90/90 | 0.986 · 90/90 |
| `ipc-2011/visit-all-sequential-satisficing` | 0.833 · 90/90 | 0.909 · 90/90 |
| `ipc-2011/woodworking-sequential-satisficing` | 0.823 · 0/90 | 0.856 · 0/90 |
| `ipc-2023-numeric/counters` | 0.667 · 72/90 | 0.750 · 90/90 |
| `ipc-2023-numeric/zenotravel` | 0.832 · 19/90 | 0.890 · 90/90 |

- **Woodworking** uses domain constants (`natural`, `untreated`…), which PlanMiner does not learn
  yet: PyPlanner rejects the problems because the learned domain does not declare them.
- **What stays below F 1** without noise is mostly ambiguity in the data (preconditions that
  always hold in the traces, see [How PlanMiner learns](algorithm.md#limits)) and equivalent
  forms the comparison does not recognize: with integers, `(< (value ?0) (max_int))` is
  `(<= (+ (value ?0) 1) (max_int))`.
- **With noise**, almost nothing works yet.
