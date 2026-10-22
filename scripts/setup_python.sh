#!/usr/bin/env bash
# Crea el entorno de Python de los tests en .venv:
# - PyPlanner (tests/planner/requirements.txt), para planificar;
# - pddldomains, del submódulo vendor/PDDLDomains, para las trazas. Se instala editable para que
#   encuentre su catálogo (vendor/PDDLDomains/dominios).
# Después hay que volver a configurar CMake.
#
# Uso: scripts/setup_python.sh [python]   (por defecto, python3; hace falta 3.12 o superior)
set -euo pipefail

PYTHON="${1:-python3}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENV="$ROOT/.venv"

if [ ! -f "$ROOT/vendor/PDDLDomains/pyproject.toml" ]; then
    git -C "$ROOT" submodule update --init vendor/PDDLDomains
fi

"$PYTHON" -m venv "$VENV"
"$VENV/bin/python" -m pip install --quiet --upgrade pip
"$VENV/bin/python" -m pip install --quiet -r "$ROOT/tests/planner/requirements.txt"
"$VENV/bin/python" -m pip install --quiet -e "$ROOT/vendor/PDDLDomains"
echo "Entorno listo en $VENV ($("$VENV/bin/python" -m pyplanner --version)," \
     "$("$VENV/bin/python" -m pddldomains --version))"
