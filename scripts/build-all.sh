#!/usr/bin/env bash
set -euo pipefail
cmake -S cpp -B build/cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build/cpp -j
(cd java-mission-server && ./mvnw test)
(cd python-tools && python -m pip install -r requirements.txt && pytest -q)
