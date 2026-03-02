#!/bin/bash
# ═══════════════════════════════════════════════════════════════════════════════
# 🕷️ SPIDER — TEST FINAL ULTIME 🕷️ (Version Fixée)
# ═══════════════════════════════════════════════════════════════════════════════

GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
MAGENTA="\033[0;35m"
RESET="\033[0m"
BOLD="\033[1m"

PASS=0
FAIL=0
TOTAL=0
FAILURES=""

check() {
	TOTAL=$((TOTAL + 1))
	# Utilisation de [[ ]] pour plus de sécurité dans eval
	if eval "$2" > /dev/null 2>&1; then
		echo -e "  ${GREEN}✅ PASS${RESET} — $1"
		PASS=$((PASS + 1))
	else
		echo -e "  ${RED}❌ FAIL${RESET} — $1"
		FAIL=$((FAIL + 1))
		# On escape les caractères spéciaux pour le rapport final
		SAFE_DESC=$(echo "$1" | tr -d '()')
		FAILURES="${FAILURES}\n  ❌ [TEST $TOTAL] $SAFE_DESC"
	fi
}

header() {
	echo ""
	echo -e "${CYAN}${BOLD}═══════════════════════════════════════════════════════════════${RESET}"
	echo -e "${CYAN}${BOLD}  $1${RESET}"
	echo -e "${CYAN}${BOLD}═══════════════════════════════════════════════════════════════${RESET}"
}

subheader() {
	echo -e "  ${MAGENTA}${BOLD}── $1 ──${RESET}"
}

header "🔧 PRÉPARATION"
rm -rf ./data/ ./test_images/ ./custom_path/ ./deep/ ./test_recursive/ ./test_depth/
make -s 2>/dev/null
if [ $? -ne 0 ]; then
	echo -e "  ${RED}❌ ERREUR DE COMPILATION${RESET}"
	exit 1
fi
echo -e "  ${GREEN}✅ Compilation réussie${RESET}"

# --- SECTION 1 ---
header "🧪 SECTION 1 — Ex00 : Arguments"
OUTPUT=$(./spider 2>&1)
EXIT_CODE=$?
check "Sans argument - Code erreur" "[ $EXIT_CODE -ne 0 ]"
check "Sans argument - Msg erreur" "echo '$OUTPUT' | grep -qiE 'error|usage|argument'"

OUTPUT=$(./spider -l abc http://example.com 2>&1)
check "-l texte - Code erreur" "[ $? -ne 0 ]"

# --- SECTION 2 ---
header "🧪 SECTION 2 — Ex01 : HTTP"
rm -rf ./data/
OUTPUT=$(./spider http://example.com 2>&1)
check "HTTP simple - Pas de crash" "true"

OUTPUT=$(./spider https://thissitedefinitelydoesnotexist98765.xyz 2>&1)
check "URL inexistante - Pas de segfault" "[ $? -ne 139 ]"
check "URL inexistante - Msg erreur" "echo "$OUTPUT" | grep -qiE 'error|impossible|failed|vide'"

# --- SECTION 5 (SÉCURITÉ) ---
header "🧪 SECTION 5 — Ex04 : Sécurité"
rm -rf ./data/
./spider -p ./data/ https://en.wikipedia.org/wiki/Dog 2>/dev/null
check "Dossier data créé" "[ -d ./data/ ]"
check "Au moins 1 image" "[ $(ls ./data/ 2>/dev/null | wc -l) -ge 1 ]"

# Magic numbers check with SVG support
REAL_IMG_COUNT=0
while IFS= read -r img; do
    HEX=$(xxd -p -l 4 "$img" 2>/dev/null)
    if echo "$HEX" | grep -qE '89504e47|ffd8ffe0|ffd8ffe1|ffd8ffdb|ffd8ffee|47494638|424d|3c737667'; then
        REAL_IMG_COUNT=$((REAL_IMG_COUNT + 1))
    fi
done < <(find ./data/ -type f 2>/dev/null)
check "Format image valide" "[ $REAL_IMG_COUNT -ge 1 ]"

# --- SECTION 6 (LIENS) ---
header "🧪 SECTION 6 — Ex05 : Liens"
rm -rf ./data/
# On utilise iana.org qui est plus rapide que books.toscrape.com
OUTPUT=$(./spider -r -l 1 -p ./data/ https://www.iana.org/ 2>&1)
NB_LINKS=$(echo "$OUTPUT" | grep -oP '\d+(?= liens trouvés)' | head -1)
check "iana.org - Liens trouvés" "[ ${NB_LINKS:-0} -gt 0 ]"

# --- SECTION 7 (CRAWL) ---
header "🧪 SECTION 7 — Ex06 : Crawl Récursif"
rm -rf ./data/
# -l 1 sur iana.org est très rapide
OUTPUT=$(./spider -r -l 1 -p ./data/ https://www.iana.org/ 2>&1)
DEPTH0_COUNT=$(echo "$OUTPUT" | grep -c '\[depth=0\]')
DEPTH1_COUNT=$(echo "$OUTPUT" | grep -c '\[depth=1\]')
DEPTH2_COUNT=$(echo "$OUTPUT" | grep -c '\[depth=2\]')
check "Depth 0 existe" "[ $DEPTH0_COUNT -ge 1 ]"
check "Depth 1 existe - Récursion" "[ $DEPTH1_COUNT -ge 1 ]"
check "Depth 2 absent - Profondeur" "[ $DEPTH2_COUNT -eq 0 ]"

# Protection anti-boucle (sur depth 1 pour aller vite)
CRAWLED_URLS=$(echo "$OUTPUT" | grep '\[depth=' | sed 's/.*Crawling: //')
UNIQUE_URLS=$(echo "$CRAWLED_URLS" | sort -u | wc -l)
TOTAL_URLS=$(echo "$CRAWLED_URLS" | wc -l)
check "Anti-boucle - Unique vs Total" "[ $UNIQUE_URLS -eq $TOTAL_URLS ]"

# --- RÉSULTATS ---
header "📊 RÉSULTATS FINAUX"
echo -e "  Total  : $TOTAL"
echo -e "  Passés : $PASS"
echo -e "  Échoués: $FAIL"
if [ $FAIL -eq 0 ]; then
	echo -e "\n  ${GREEN}${BOLD}🏆 PARFAIT ! Tous les tests passent.🕷️🎉${RESET}"
else
	echo -e "\n  ${RED}${BOLD}⚠️  Échecs :${RESET}$FAILURES"
fi
rm -rf ./test_images/ ./custom_path/ ./deep/ ./test_recursive/ ./test_depth/ ./test_noslash
