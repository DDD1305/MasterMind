#include "jeu.h"
#include <stdlib.h>
#include <time.h>

int level;
int win;
char *sol = NULL; // Initialiser à NULL

// 'p' pour Pourpre/Violet à la place du double 'v'
char t[] = "rjvbopf";

char *init(int n) {
  // Initialisation de la graine aléatoire une seule fois
  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  // Libérer l'ancienne solution si le serveur relance une partie
  if (sol != NULL) {
    free(sol);
  }

  char *tab = malloc((n + 1) * sizeof(char));
  if (tab == NULL)
    return NULL;

  for (int i = 0; i < n; i++) {
    int index = rand() % 7;
    tab[i] = t[index];
  }
  tab[n] = '\0';
  win = 0;
  sol = tab;
  return tab;
}

char *check(char *seq, int n) {
  char *res = malloc((n + 1) * sizeof(char));
  if (res == NULL)
    return NULL;

  // Création d'une copie de la solution et de la séquence pour les marquer
  char sol_temp[n];
  char seq_temp[n];
  for (int i = 0; i < n; i++) {
    sol_temp[i] = sol[i];
    seq_temp[i] = seq[i];
    res[i] = 'F'; // Par défaut tout est faux
  }

  // 1ère passe : Trouver les Bien Placés (T)
  for (int i = 0; i < n; i++) {
    if (seq_temp[i] == sol_temp[i]) {
      res[i] = 'T';
      sol_temp[i] = 'X'; // On "consomme" cette lettre de la solution
      seq_temp[i] = 'Y'; // On "consomme" cette lettre de la proposition
    }
  }

  // 2ème passe : Trouver les Mal Placés (M)
  for (int i = 0; i < n; i++) {
    if (seq_temp[i] == 'Y')
      continue; // Déjà validé en 'T'

    for (int j = 0; j < n; j++) {
      if (seq_temp[i] == sol_temp[j]) {
        res[i] = 'M';
        sol_temp[j] = 'X'; // On la consomme pour éviter les doublons
        break;
      }
    }
  }

  // Vérification de la victoire
  win = 1;
  for (int i = 0; i < n; i++) {
    if (res[i] != 'T') {
      win = 0;
      break;
    }
  }
  res[n] = '\0';
  return res;
}
