# Licenses

**English** · [Español](LICENCIAS.md)

The code of this repository (the command line, the libraries in `libs/`, the tests and the
scripts), the documentation and the benchmark definitions are distributed under the
[MIT license](LICENSE).

## INSLV

[INSLV](https://github.com/Leontes/INSLV-PlanMiner) is not in this repository: it is downloaded
when configuring. It is based on NSLV, by Raúl Pérez (© 2002–2003), and is distributed under the
GNU General Public License, version 3 (`GPL-3.0-only`). The MIT license does **not** apply to
INSLV.

The `planminer` executable and the libraries that link INSLV (`rules` and `pddl`) form a
combined work with it: built and distributed, they are subject to the GPL-3.0. The rest of the
code can be used on its own under the MIT license.

## Test domains and traces

- The PDDLDomains catalog (the `vendor/PDDLDomains` submodule) has its own terms: see its
  `LICENSES.md`.
- The test traces (`tests/data/traces`) come from domains and problems of that catalog, and the
  files in `tests/data/blocksworld`, from the IPC blocksworld domain: they follow the terms of
  their originals.
