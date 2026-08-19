#!/bin/bash
BIN=./codexion

rate() {
    local total=0
    for i in $(seq 1 $2); do
        total=$((total + $($BIN $1 2>&1 | tail -1 | grep -c "burned out")))
    done
    echo "$total/$2"
}

echo "=== EDF serrés (0 attendu) ==="
for n in 4 5 7 9; do echo "N=$n : $(rate "$n 3000 200 200 200 10 800 edf" 3)"; done

echo "=== FIFO serrés (0 attendu) ==="
for n in 4 5 7; do echo "N=$n : $(rate "$n 3000 200 200 200 10 800 fifo" 3)"; done

echo "=== Marges larges rapides (0 attendu) ==="
echo "edf : $(rate "7 5000 50 20 20 3 20 edf" 3)"
echo "fifo: $(rate "5 5000 50 20 20 3 20 fifo" 3)"

echo "=== Burnout légitime (burned out attendu) ==="
$BIN 1 1 1 1 1 1 1 edf 2>&1 | tail -1

echo "=== Deadlock (OK attendu, 10x) ==="
ok=0
for i in $(seq 1 3); do timeout 15 $BIN 7 5000 50 20 20 3 20 edf >/dev/null 2>&1 && ok=$((ok+1)); done
echo "OK=$ok/3"

echo "=== Parsing (erreurs attendues) ==="
$BIN 2>&1 | head -1
$BIN 5 3000 200 200 200 10 800 2>&1 | head -1
$BIN 5 3000 200 200 200 10 800 xyz 2>&1 | head -1
$BIN abc 3000 200 200 200 10 800 edf 2>&1 | head -1

echo "=== FIN ==="