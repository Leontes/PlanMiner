# How PlanMiner learns

**English** · [Español](../es/algoritmo.md)

PlanMiner learns each action on its own, from its applications in the traces: the state before
each one, the state after it and its arguments. This document describes the parts of the
algorithm and what the improved configuration changes with respect to the original PlanMiner.

## The parts

1. **The traces** (`libs/plantraces`). PlanMiner reads the traces (`plantraces::read_trace`) and
   groups the applications of each action. An atom that is neither among the true ones nor among
   the unknown ones is false, also in open world. The type hierarchy is inferred from the
   objects in the traces: if a parameter appears with two types, they get a common parent.
2. **The example table** (`datasets::build_table`). For each action, one row per state (the ones
   before each application and, after them, the ones after) and one column per atom and fluent
   that can be expressed with its parameters: `(on ?0 ?1)`, `(fuel ?0)`. An object that appears
   in several parameters is expressed with the first one.
3. **Numeric effects** (`datasets::numeric_effects`, `libs/regression`). For each fluent that
   changes, an A* search for the arithmetic expression of its change, with the other fluents and
   constants, until the error (MAPE) drops below 2 %: `(* (distance ?1 ?2) (slow-burn ?0))`.
4. **Numeric relations** (`datasets::numeric_relations`). Comparisons (`=`, `<`, `>`) between
   fluents and with the expressions of the effects, added to the table as true or false
   attributes. The numeric preconditions come from them.
5. **The rules** (`libs/rules`, [INSLV](https://github.com/Leontes/INSLV-PlanMiner)). Two example
   sets per action: the logical one, with the atoms, and the numeric one, with the fluent values
   (discretized) and the relations. The class of each row is the state before or after. INSLV learns
   fuzzy rules that describe each class.
6. **The fusion** (`rules::fuse`). The conditions of the rules of the state before form the
   precondition, and those of the state after that were not there before, the effect.
7. **The writing** (`pddl::write`). The domain in PDDL, with the types, predicates, functions
   and actions.

`pddl::learn` does the whole process; `pddl::Options` picks each improvement.

## The improved configuration

The original configuration (`--original`, `pddl::Options::original()`) reproduces the original
PlanMiner: with the same traces, it gives the same domain. The improved one (the default,
`pddl::Options{}`) fixes its flaws and adds what it lacked. Each improvement is a field of the
options, so that it can be measured on its own.

### Numeric effects

| Improvement | What it does | Option |
|-------------|--------------|--------|
| State before | The variables of the expression are the fluents of the state before, including its own (the PDDL semantics). The original used those of the state after, without its own. | `EffectOptions::before_state` |
| Effect forms | It searches for `increase`, `decrease` and `assign`, with their sign. The original searched for the absolute value of the change and left the sign to INSLV. | `EffectOptions::effect_forms` |
| Null model | A fluent that does not change in at least 75 % of the applications has no effect, even if noise alters it in some. | `EffectOptions::null_model` |
| Constant folding | `(* 10 3)` is written `30`. | `EffectOptions::fold_constants` |
| Search | Tree successors (the original only grew on the right and could not express `a * (b + c)`), duplicates by their values, constants from 1 to 10, no constant variables and a maximum size. | `regression::Options` |
| Effects from the regression | The numeric effects come from the regression, with their form and sign. The original gave INSLV the sign of the change and turned conditions on discretized values into assignments. | `ExampleOptions::changes` |

### Numeric preconditions

| Improvement | What it does | Option |
|-------------|--------------|--------|
| Effect expressions | Each fluent is compared with the expressions of the action's effects: `(>= (fuel ?0) (* (distance ?1 ?2) (slow-burn ?0)))`. | `RelationOptions::effect_expressions` |
| Changing pairs | Every pair where some side changes (with the null model) is compared, in any order. | `RelationOptions::changing_pairs` |
| Constants | Relations that have the same value in every row where they are known are discarded. | `RelationOptions::known_constants` |
| No time | The time is not a fluent: it is neither compared nor an attribute. | `RelationOptions::time`, `ExampleOptions::time` |
| No costs or accumulators | `(total-cost)` and the fluents that never decrease (such as `total-fuel-used`) are not compared: their relations depend on when the action is applied. | `RelationOptions::action_costs`, `RelationOptions::accumulators` |
| Constant bounds | Relations that have the same value in every row of an action are not passed to INSLV, because they would add coincidences. From those and the others, bounds (`<=`, `>=`) that hold in every state before the action, are reached in some and are not invariants are preconditions: `(<= (onboard ?0) (zoom-limit ?0))`. Only without noise. | `Options::contrast` |

### Rules and fusion

| Improvement | What it does | Option |
|-------------|--------------|--------|
| Repeated parameters | In an application with an object in several parameters (`move ?from ?to` with `?from = ?to`), the atoms of those parameters are unknown: a delete and an add of the same atom cancel out and the effect cannot be seen. | `Options::aliasing` |
| Validation | Each condition of a rule stays if it holds in at least 95 % of the examples of its state. | `FusionOptions::validate` |
| Exact validation | If the traces have no noise, validation requires every example. Noise is estimated from what changes outside each action's arguments (`plantraces::frame_changes`). | `Options::exact_without_noise` |
| Majority | In a conflict between two values of an attribute, the one that covers more examples wins. | `FusionOptions::majority` |
| Combined comparisons | The comparisons of a pair, true and false, are combined into one (`<`, `<=`, `=`, `>=`, `>`). | `FusionOptions::combine_comparisons` |
| Redundant preconditions | Comparisons implied by the other conditions in the states of the traces are removed, and so are atoms equivalent to another one of the same relation: `(connected ?1 ?0)` with `(connected ?0 ?1)`. | `Options::drop_implied` |

### Noise (PlanMiner-N)

The techniques of PlanMiner-N, the extension of PlanMiner for noisy traces. They only act when
noise is estimated (`plantraces::frame_changes`, c > 0): without noise, the domain is the same.

| Improvement | What it does | Option |
|-------------|--------------|--------|
| Rare values | In each atom and each class (before or after the action), the value that appears in less than 2c of the rows becomes unknown, capped at 0.5. INSLV can then describe the state with that condition at no cost, and validation does not count those values. It is PlanMiner-N's statistical filter, with a threshold proportional to the noise instead of a fixed 5 %, which with 5 % noise only cleans half of it. As an option, the threshold of a binomial: with p the noise of each observation (c = 2p(1 - p)) and n rows, the value that appears in m rows or fewer, with m the smallest such that P(Binomial(n, p) > m) ≤ α. With many rows it approaches p and keeps the rare values that are true; with few, it rises. | `Options::rare_values`, `Options::rare_values_per_noise`, `Options::rare_values_alpha` |
| Fluent smoothing | The values of each fluent are grouped with a top-down hierarchical clustering (HRC: k-means with k = 2, repeated in each cluster while its dispersion, 0.6 (1 - silhouette) + 0.4 σ/μ, exceeds 0.05) and replaced by the mean of their cluster; single-value clusters are outliers and become unknown. Before searching for the effects and the relations. Except `(total-cost)` and accumulators, which do not repeat. | `Options::smooth_fluents` |
| Meta-state refinement | The fusion drops the conditions whose support (the examples covered by the rules that have them) is below 5 % of the applications, and in a conflict whose support difference is within 10 % of their mean, both values. | `Options::refine_meta_states` |

### Writing

| Improvement | What it does | Option |
|-------------|--------------|--------|
| `:numeric-fluents` | The domain declares it if it has functions. Without it, some planners reject the domain. | `Options::numeric_fluents` |
| Equality | Equalities are written with `=`. The original writes `==`, which is not PDDL. | `Options::equality` |

## Limits

- **Language:** STRIPS with types and action costs, and numeric fluents without durative
  actions. No ADL (quantifiers, conditional effects, disjunctions), no domain constants, no
  durative actions.
- **Noise:** with 5 % noise in the traces, essential literals go missing and almost no learned
  domain gives valid plans.
- **Ambiguity:** there are only examples of what happens, not of what cannot happen. A
  precondition that holds in every state of the traces where the action is applied cannot be
  ruled out with them: a static relation (`(reachable-floor ?lift ?floor)`), a state invariant
  (`(visited ?from)`, where the robot is) or a bound the plan never exceeds. They do not break
  the plans, but they lower the precision.
