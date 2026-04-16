/******************************************************************************/
/*			Application: ... */
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/

#include <curses.h> /* Primitives de gestion d'ecran */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>

#include "fon.h" /* primitives de la boite a outils */

#include "jeu.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli(char *serveur, char *service);

/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[]) {

  char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
  char *service =
      SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc) {
  case 1: /* arguments par defaut */
    printf("serveur par defaut: %s\n", serveur);
    printf("service par defaut: %s\n", service);
    break;
  case 2: /* serveur renseigne  */
    serveur = argv[1];
    printf("service par defaut: %s\n", service);
    break;
  case 3: /* serveur, service renseignes */
    serveur = argv[1];
    service = argv[2];
    break;
  default:
    printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
    exit(1);
  }

  /* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
  /* service le numero de port sur le serveur correspondant au  */
  /* service desire par le client */

  client_appli(serveur, service);
}

/*****************************************************************************/
void client_appli(char *serveur, char *service)

/* procedure correspondant au traitement du client de votre application */

{
  int socket = h_socket(AF_INET, SOCK_STREAM);
  struct sockaddr_in *p_adr_socket;
  adr_socket(service, serveur, SOCK_STREAM, &p_adr_socket);
  h_connect(socket, p_adr_socket);
  int taille_buff = n;
  char *tampon_read = malloc(taille_buff);
  char *tampon_write = malloc(taille_buff);
  int niveau;
  printf("Entrer le niveau : ");
  scanf("%d", &niveau);
  int niveau_reseau = htonl(niveau);
  h_writes(socket, (char *)&niveau_reseau, sizeof(int));
  int reads = 1;
  int partie_gagnee = 0;
  
  while (!partie_gagnee) {
    // 1. Lire l'invitation du serveur ("Entrer la proposition" - 21 octets)
    reads = h_reads(socket, tampon_read, 21);
    if (reads <= 0) break;
    printf("%.*s\n", reads, tampon_read);

    // 2. Saisir la proposition (attention : l'utilisateur doit taper tout attaché, ex: "RVBB")
    scanf("%s", tampon_write);

    // 3. Envoyer UNIQUEMENT la taille attendue par le serveur (niveau)
    h_writes(socket, tampon_write, niveau);

    // 4. Lire la réponse du serveur (qui fait exactement 'niveau' octets)
    reads = h_reads(socket, tampon_read, niveau);
    if (reads <= 0) break;
    printf("Résultat : %.*s\n\n", niveau, tampon_read);

    // 5. Vérifier localement si la partie est gagnée (uniquement des 'T')
    partie_gagnee = 1;
    for (int i = 0; i < niveau; i++) {
      if (tampon_read[i] != 'T') {
        partie_gagnee = 0;
        break;
      }
    }
    
    if (partie_gagnee) {
      printf("Bravo, vous avez trouvé la bonne combinaison !\n");
    }
  }

  h_close(socket);
  free(tampon_read);
  free(tampon_write);
}

/*****************************************************************************/
