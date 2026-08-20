#!/bin/bash
# ============================================================================
#  Test du non-determinisme de l'ordonnancement EDF dans Codexion
# ----------------------------------------------------------------------------
#  Idee : un programme concurrent correct n'est PAS deterministe dans son
#  deroule (l'OS ordonnance les threads librement), mais il GARANTIT ses
#  invariants (ici : personne ne burn out sur des parametres faisables, et
#  tout le monde termine). Ce script met les deux faces en evidence.
# ============================================================================

BIN=./codexion
ARGS="5 3000 100 100 100 3 100 edf"

echo "============================================================"
echo " Commande testee : ./codexion $ARGS"
echo "============================================================"
echo ""

# ---------------------------------------------------------------------------
echo "[1] NON-DETERMINISME : les traces varient d'un run a l'autre"
echo "    On lance 8 fois la MEME commande et on compare les empreintes"
echo "    (md5) des sorties. Si l'ordonnancement etait fige, toutes les"
echo "    empreintes seraient identiques. Plusieurs empreintes = l'OS"
echo "    ordonnance les threads differemment a chaque execution."
echo ""
tmp=$(mktemp)
for i in $(seq 1 8); do
    $BIN $ARGS 2>&1 | md5sum | cut -c1-12 >> "$tmp"
    echo "    run $i : $(tail -1 "$tmp")"
done
uniq=$(sort -u "$tmp" | wc -l | tr -d ' ')
rm -f "$tmp"
echo ""
echo "    -> $uniq empreintes differentes sur 8 runs."
echo "       (>1 confirme que l'ordonnancement est non-deterministe)"
echo ""

# ---------------------------------------------------------------------------
echo "[2] CORRECTION MALGRE LE NON-DETERMINISME : 0 burnout attendu"
echo "    Meme si l'ordre change a chaque fois, l'invariant doit tenir :"
echo "    sur des parametres faisables, aucun coder ne doit burn out."
echo ""
burn=0
for i in $(seq 1 20); do
    burn=$((burn + $($BIN $ARGS 2>&1 | tail -1 | grep -c "burned out")))
done
echo "    -> burnout : $burn/20"
if [ "$burn" -eq 0 ]; then
    echo "       OK : correct sur les 20 runs malgre l'ordre variable."
else
    echo "       ATTENTION : $burn run(s) ont burn out, a investiguer."
fi
echo ""

# ---------------------------------------------------------------------------
echo "[3] L'ORDRE VISIBLE CHANGE : premiers coders a compiler"
echo "    On affiche, pour chaque run, l'ordre des 3 premiers 'is compiling'."
echo "    Il change d'un run a l'autre : c'est le non-determinisme rendu"
echo "    concret, coder par coder."
echo ""
for i in $(seq 1 6); do
    order=$($BIN $ARGS 2>&1 | grep 'is compiling' | head -3 | awk '{print $2}' | paste -sd,)
    echo "    run $i : premiers a compiler = $order"
done
echo ""

echo "============================================================"
echo " Conclusion : le deroule varie (sections 1 et 3), mais la"
echo " correction est garantie (section 2). C'est le comportement"
echo " attendu d'un programme concurrent correct."
echo "============================================================"