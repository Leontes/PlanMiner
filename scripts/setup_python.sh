#!/usr/bin/env bash
# Crea el entorno de Python de los tests del planificador en .venv, con PyPlanner
# (tests/planner/requirements.txt). Después hay que volver a configurar CMake.
#
# Uso: scripts/setup_python.sh [python]   (por defecto, python3; hace falta 3.12 o superior)
set -euo pipefail

PYTHON="${1:-python3}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VENV="$ROOT/.venv"

"$PYTHON" -m venv "$VENV"
"$VENV/bin/python" -m pip install --quiet --upgrade pip
"$VENV/bin/python" -m pip install --quiet -r "$ROOT/tests/planner/requirements.txt"
echo "Entorno listo en $VENV ($("$VENV/bin/python" -m pyplanner --version))"
