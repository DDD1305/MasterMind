#!/usr/bin/env bash

# Script de test automatique pour le TP Mastermind client/serveur.
# Il lance le serveur en arrière-plan, lance le client avec des entrées simulées,
# puis ferme le serveur proprement.

set -u

SERVICE="${1:-1111}"
SERVEUR="${2:-127.0.0.1}"

SERVER_EXEC="./serveur"
CLIENT_EXEC="./client"

if [[ ! -x "$SERVER_EXEC" ]]; then
    echo "Erreur : $SERVER_EXEC introuvable ou non exécutable."
    echo "Compile d'abord avec : make"
    exit 1
fi

if [[ ! -x "$CLIENT_EXEC" ]]; then
    echo "Erreur : $CLIENT_EXEC introuvable ou non exécutable."
    echo "Compile d'abord avec : make"
    exit 1
fi

cleanup() {
    if [[ -n "${SERVER_PID:-}" ]] && kill -0 "$SERVER_PID" 2>/dev/null; then
        echo "Arrêt du serveur..."
        kill "$SERVER_PID" 2>/dev/null
        wait "$SERVER_PID" 2>/dev/null
    fi
}

trap cleanup EXIT INT TERM

echo "=== Lancement du serveur sur le service $SERVICE ==="
"$SERVER_EXEC" "$SERVICE" &
SERVER_PID=$!

# Laisse au serveur le temps de faire socket/bind/listen
sleep 1

echo "=== Lancement du client ==="

# Entrées simulées :
# 4     -> niveau
# RVBB  -> proposition 1
# RVRB  -> proposition 2
# BBBB  -> proposition 3
#
# Adapte les propositions selon les couleurs attendues par ton jeu.

(
echo 2

t="rjvbopf"
for ((i=0; i<${#t}; i++)); do
  for ((j=0; j<${#t}; j++)); do
    echo "${t:i:1}${t:j:1}"
  done
done
) | timeout 10s "$CLIENT_EXEC" "$SERVEUR" "$SERVICE"

CLIENT_STATUS=$?

if [[ "$CLIENT_STATUS" -eq 0 ]]; then
    echo "=== Test terminé : client terminé normalement ==="
else
    echo "=== Test terminé : le client a retourné $CLIENT_STATUS ==="
fi

exit "$CLIENT_STATUS"
