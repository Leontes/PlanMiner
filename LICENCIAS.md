# Licencias

[English](LICENSES.md) · **Español**

El código de este repositorio (la línea de comandos, las librerías de `libs/`, los tests y los
scripts), la documentación y las definiciones del banco de pruebas se distribuyen con la
[licencia MIT](LICENSE).

## INSLV

`vendor/inslv` parte de NSLV, de Raúl Pérez (© 2002–2003), con la licencia GNU GPL versión 2 o
posterior, como dicen las cabeceras de sus fuentes. La licencia MIT **no** se aplica a INSLV:
sigue con la GPL ([texto de la versión 3](vendor/inslv/LICENSE)), también los cambios que se le
han hecho aquí.

El ejecutable `planminer` y las librerías que enlazan INSLV (`rules` y `pddl`) forman una obra
combinada con él: compilados y distribuidos, quedan sujetos a la GPL. El resto del código se
puede usar por separado con la licencia MIT.

## Dominios y trazas de prueba

- El catálogo PDDLDomains (submódulo `vendor/PDDLDomains`) tiene sus propias condiciones: ver
  su `LICENCIAS.md`.
- Las trazas de prueba (`tests/data/traces`) salen de dominios y problemas de ese catálogo, y
  los ficheros de `tests/data/blocksworld`, del dominio de blocksworld de la IPC: siguen las
  condiciones de sus originales.
