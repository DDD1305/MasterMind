#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// niveau
int n;
// win =0 jeu en cours
int win;

char t[] = "rjvbovf";

char *init(int n) {
  char *tab = malloc((n + 1) * sizeof(char));
  if (tab == NULL)
    return NULL;

  for (int i = 0; i < n; i++) {
    int index = rand() % 7;
    tab[i] = t[index];
  }
  tab[n] = '\0';
  win = 0;
  return tab;
}

char *check(char *seq, char *sol, int n) {
  char *res = malloc((n + 1) * sizeof(char));
  if (res == NULL)
    return NULL;

  for (int i = 0; i < n; i++) {
    res[i] = 'F';

    if (seq[i] == sol[i]) {
      res[i] = 'T';
    } else {
      for (int j = 0; j < n; j++) {
        if (seq[i] == sol[j]) {
          res[i] = 'M';
          break;
        }
      }
    }
  }
  win = 1;
  for (int i = 0; i < n; i++) {
    if (res[i] != 'T') {
      win = 0;
    }
  }
  res[n] = '\0';
  return res;
}
/*
int main() {
  srand(time(NULL));

  char *sol = init(n);
  printf("Solution : %s\n", sol);

  char entree[n];
  printf("Entrez votre sequence : ");
  scanf("%49s", entree);

  char *verif = check(entree, sol, n);

  for (int i = 0; i < n; i++) {
    printf("Position %d : %c\n", i, verif[i]);
  }

  free(sol);
  free(verif);

  return 0;
}*/