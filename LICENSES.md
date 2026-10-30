# Licenses

**English** · [Español](LICENCIAS.md)

The code of this repository (the command line, the libraries in `libs/`, the tests and the
scripts), the documentation and the benchmark definitions are distributed under the
[MIT license](LICENSE).

## INSLV

`vendor/inslv` is based on NSLV, by Raúl Pérez (© 2002–2003), under the GNU GPL version 2 or
later, as stated in the headers of its sources. The MIT license does **not** apply to INSLV: it
stays under the GPL ([text of version 3](vendor/inslv/LICENSE)), including the changes made to
it here.

The `planminer` executable and the libraries that link INSLV (`rules` and `pddl`) form a
combined work with it: built and distributed, they are subject to the GPL. The rest of the code
can be used on its own under the MIT license.

## Test domains and traces

- The PDDLDomains catalog (the `vendor/PDDLDomains` submodule) has its own terms: see its
  `LICENSES.md`.
- The test traces (`tests/data/traces`) come from domains and problems of that catalog, and the
  files in `tests/data/blocksworld`, from the IPC blocksworld domain: they follow the terms of
  their originals.
