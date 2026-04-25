# Mastermind

## Description

Ce projet implémente un jeu de Mastermind en architecture client/serveur en C, utilisant des sockets TCP.

- Le serveur gère la logique du jeu
- Le client permet à l’utilisateur de jouer en envoyant des propositions

## Compilation

Utiliser le Makefile fourni :

```bash
Pour générer les binaires:
make

Pour nettoyer :
make clean

Lancement:
1. Lancer le serveur
./serveur
2. Lancer le client
./client
```

Fonctionnement: 

Le client choisit un niveau (longueur de la combinaison)
Le serveur génère une combinaison secrète
Le client envoie des propositions
Le serveur répond avec une correction
Le jeu continue jusqu’à trouver la bonne combinaison

Protocole:

Les messages échangés suivent ce format :

Envoi de la taille du message (int en format réseau)
Envoi du message (sans le caractère '\0')

Types de messages :

Consigne (ex : "Entrer la proposition")
Proposition du client
Correction du serveur
État du jeu ("WIN")

Concepts utilisés:
Sockets TCP (socket, bind, listen, accept, connect)
Communication réseau (endianness avec htonl / ntohl)
Gestion des buffers et chaînes en C
Synchronisation client/serveur


Test:

Un script bash permet de tester automatiquement :

`./test_mastermind.sh`
