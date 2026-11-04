# Licencias

[English](LICENSES.md) · **Español**

El código de este repositorio (la línea de comandos, las librerías de `libs/`, los tests y los
scripts), la documentación y las definiciones del banco de pruebas se distribuyen con la
[licencia MIT](LICENSE).

## INSLV

[INSLV](https://github.com/Leontes/INSLV-PlanMiner) no está en este repositorio: se descarga al
configurar. Parte de NSLV, de Raúl Pérez (© 2002–2003), y se distribuye con la licencia pública
general de GNU, versión 3 (`GPL-3.0-only`). La licencia MIT **no** se aplica a INSLV.

El ejecutable `planminer` y las librerías que enlazan INSLV (`rules` y `pddl`) forman una obra
combinada con él: compilados y distribuidos, quedan sujetos a la GPL-3.0. El resto del código se
puede usar por separado con la licencia MIT.

## Dominios y trazas de prueba

- El catálogo PDDLDomains (submódulo `vendor/PDDLDomains`) tiene sus propias condiciones: ver
  su `LICENCIAS.md`.
- Las trazas de prueba (`tests/data/traces`) salen de dominios y problemas de ese catálogo, y
  los ficheros de `tests/data/blocksworld`, del dominio de blocksworld de la IPC: siguen las
  condiciones de sus originales.
