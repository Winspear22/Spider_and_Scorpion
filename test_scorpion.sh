#!/bin/bash
# ============================================================================
# test_scorpion.sh — Testeur d'intégration Spider + Scorpion (REAL IMAGES)
#
# Ce script :
#   1. Compile spider et scorpion
#   2. Télécharge des images RÉELLES avec Spider
#   3. Valide les métadonnées avec Scorpion
# ============================================================================

# Couleurs
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
RESET="\033[0m"

PASS=0
FAIL=0
TOTAL=0
DOWNLOAD_DIR="/tmp/scorpion_test_samples/"

# Fonction : exécuter un test et vérifier le résultat
run_test()
{
    local test_name="$1"
    local command="$2"
    local expect_success="$3"
    local expect_output="$4"

    TOTAL=$((TOTAL + 1))
    echo -e "${CYAN}──── Test $TOTAL : $test_name ────${RESET}"

    output=$(eval "$command" 2>&1)
    exit_code=$?

    if [ "$expect_success" = "yes" ] && [ "$exit_code" -ne 0 ]; then
        echo -e "${RED}  ✗ FAIL${RESET} — Code $exit_code"
        echo "  Output: $output"
        FAIL=$((FAIL + 1))
        return
    fi

    if [ -n "$expect_output" ]; then
        if echo "$output" | grep -qi "$expect_output"; then
            echo -e "${GREEN}  ✓ PASS${RESET} — \"$expect_output\" trouvé"
            PASS=$((PASS + 1))
        else
            echo -e "${RED}  ✗ FAIL${RESET} — \"$expect_output\" non trouvé"
            echo "  Output: $(echo "$output" | head -n 5)"
            FAIL=$((FAIL + 1))
        fi
    else
        echo -e "${GREEN}  ✓ PASS${RESET}"
        PASS=$((PASS + 1))
    fi
}

echo -e "${YELLOW}╔══════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${YELLOW}║          TEST D'INTÉGRATION RÉEL SPIDER + SCORPION          ║${RESET}"
echo -e "${YELLOW}╚══════════════════════════════════════════════════════════════╝${RESET}"

# 0. Compilation
echo -e "\n${YELLOW}▶ Phase 0 : Compilation${RESET}\n"
make scorpion spider > /dev/null 2>&1
[ $? -eq 0 ] && echo -e "${GREEN}  ✓ Binaires prêts${RESET}" || { echo "Compilation failed"; exit 1; }

# 1. Téléchargement d'échantillons
echo -e "\n${YELLOW}▶ Phase 1 : Spider télécharge des échantillons réels${RESET}\n"
rm -rf "$DOWNLOAD_DIR" && mkdir -p "$DOWNLOAD_DIR"

# URLs ciblées par format pour garantir de vrais fichiers
TARGETS=(
  "https://www.kernel.org" # PNG (logos)
  "https://commons.wikimedia.org/wiki/Main_Page" # JPEG (photo du jour)
  "https://www.w3.org/People/mimasa/test/imgformat/" # GIF / BMP / etc.
)

for url in "${TARGETS[@]}"; do
    echo -e "${CYAN}  Spider vers $url...${RESET}"
    ./spider -l 1 -p "$DOWNLOAD_DIR" "$url" > /dev/null 2>&1
done

# Nettoyage des "faux" fichiers (certains sites renvoient de l'HTML pour des fichiers inexistants)
# On ne garde que les fichiers dont le premier octet n'est pas '<' (HTML)
find "$DOWNLOAD_DIR" -type f | while read -r line; do
    if head -c 1 "$line" | grep -q "<"; then
        rm "$line"
    fi
done

FILE_COUNT=$(find "$DOWNLOAD_DIR" -type f | wc -l)
if [ "$FILE_COUNT" -gt 0 ]; then
    echo -e "\n${GREEN}  ✓ $FILE_COUNT fichiers RÉELS téléchargés.${RESET}\n"
else
    echo -e "\n${RED}  ✗ Aucun fichier téléchargé. Vérifiez la connexion.${RESET}\n"
    exit 1
fi

# 2. Tests Scorpion
echo -e "${YELLOW}▶ Phase 2 : Scorpion analyse les fichiers réels${RESET}\n"

# Filtrer par extension
get_sample() { find "$DOWNLOAD_DIR" -type f -name "*.$1" | head -1; }

# --- TEST JPEG ---
SAMPLE_JPG=$(get_sample "jpg")
[ -z "$SAMPLE_JPG" ] && SAMPLE_JPG=$(get_sample "jpeg")
if [ -n "$SAMPLE_JPG" ]; then
    run_test "JPEG : Détection Format" "./scorpion $SAMPLE_JPG" "yes" "Type         : JPEG"
    run_test "JPEG : Métadonnées Filesystem" "./scorpion $SAMPLE_JPG" "yes" "Taille\|Modifié"
else
    echo -e "${YELLOW}  ⚠ Aucun JPEG trouvé pour ce test.${RESET}"
fi

# --- TEST PNG ---
SAMPLE_PNG=$(get_sample "png")
[ -n "$SAMPLE_PNG" ] || SAMPLE_PNG=$(get_sample "PNG")
if [ -n "$SAMPLE_PNG" ]; then
    run_test "PNG : Détection Format" "./scorpion $SAMPLE_PNG" "yes" "Type         : PNG"
    run_test "PNG : Métadonnées PNG" "./scorpion $SAMPLE_PNG" "yes" "Dimensions"
else
    echo -e "${YELLOW}  ⚠ Aucun PNG trouvé pour ce test.${RESET}"
fi

# --- TEST GIF ---
SAMPLE_GIF=$(get_sample "gif")
if [ -n "$SAMPLE_GIF" ]; then
    run_test "GIF : Détection Format" "./scorpion $SAMPLE_GIF" "yes" "Type         : GIF"
else
    echo -e "${YELLOW}  ⚠ Aucun GIF trouvé pour ce test.${RESET}"
fi

# 3. Tests de robustesse
echo -e "\n${YELLOW}▶ Phase 3 : Robustesse et fichiers multiples${RESET}\n"

run_test "Traitement multiples" "./scorpion $DOWNLOAD_DIR/*" "yes" "Type"
run_test "Fichier non supporté" "echo 'test' > /tmp/bad_file.txt && ./scorpion /tmp/bad_file.txt" "no" "not supported"

# Résumé
echo -e "\n${YELLOW}╔══════════════════════════════════════════════════════════════╗${RESET}"
echo -e "  Total Tests : $TOTAL"
echo -e "  ${GREEN}Passés      : $PASS${RESET}"
echo -e "  ${RED}Échoués     : $FAIL${RESET}"
echo -e "${YELLOW}╚══════════════════════════════════════════════════════════════╝${RESET}\n"

[ "$FAIL" -eq 0 ] && echo -e "${GREEN}  INTÉGRATION RÉELLE RÉUSSIE ! 🦂🕸️${RESET}" || echo -e "${RED}  ÉCHEC DE L'INTÉGRATION.${RESET}"
rm -f /tmp/bad_file.txt
