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
#include <unistd.h>

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

int socketClient;

int init_TCP_com(char *serveur, char *service) {
  int socket = h_socket(AF_INET, SOCK_STREAM);
  struct sockaddr_in *p_adr_socket;
  adr_socket(service, serveur, SOCK_STREAM, &p_adr_socket);
  h_connect(socket, p_adr_socket);
  return socket;
}

int ask_level(int socket) {
  int niveau;
  printf("Entrer le niveau : ");
  scanf("%d", &niveau);
  int niveau_reseau = htonl(niveau);
  int write = h_writes(socket, (char *)&niveau_reseau, sizeof(int));
  if (write != sizeof(int)) {
    return -1;
  }
  return 0;
}

int send_msg(char *msg) {
  int len = strlen(msg);
  int write = h_writes(socketClient, (char *)&len,
                       sizeof(int)); // d'abord la taille du msg
  if (write != sizeof(int)) {
    return -1;
  }
  write = h_writes(socketClient, msg, len);
  if (write != len) {
    return -1;
  }
  return write;
}

int receive_msg(char *tampon, int taille) {
  int len;
  int read = h_reads(socketClient, (char *)&len,
                     sizeof(int)); // On lit d'abord la taille
  if (read != sizeof(int)) {
    return -1;
  }
  if (len > taille) {
    printf("buffer tampon trop petit");
    return -1;
  }
  read = h_reads(socketClient, tampon, len);
  if (read != len) {
    return -1;
  }
  return read; // retourne la taille du msg lu ou -1 si erreur.
}

/*****************************************************************************/
void client_appli(char *serveur, char *service)

/* procedure correspondant au traitement du client de votre application */

{
  socketClient = init_TCP_com(serveur, service);
  int taille_buff = level + 1;
  char *tampon_read = malloc(taille_buff);
  if (tampon_read == NULL) {
    printf("Erreur allocation tampon_read");
    goto cleanup;
  }
  char *tampon_write = malloc(taille_buff);
  if (tampon_write == NULL) {
    printf("Erreur à l'allocation tampon_write");
    goto cleanup;
  }
  if (ask_level(socket) == -1) {
    printf("Erreur à la récupération du niveau\n");
  }

  while (!win) {
    // 1. Lire l'invitation du serveur ("Entrer la proposition" - 21 octets)
    int read = receive_msg(tampon_read, taille_buff);
    if (read == -1) {
      printf("Erreur réception de msg\n");
      goto cleanup;
    }
    printf("%.*s\n", read, tampon_read);

    // 2. Saisir la proposition (attention : l'utilisateur doit taper tout
    // attaché, ex: "RVBB")
    int take = 0;
    while (!take) {
      scanf("%s", tampon_write);
      if (tampon_write[taille_buff] == '\0') {
        take = 1;
      }
    }

    // 3. Envoyer UNIQUEMENT la taille attendue par le serveur (niveau)
    h_writes(socket, tampon_write, level);

    // 4. Lire la réponse du serveur (qui fait exactement 'niveau' octets)
    reads = h_reads(socket, tampon_read, level);
    if (reads <= 0)
      break;
    printf("Résultat : %.*s\n\n", level, tampon_read);
  }

  if (!win) {
    printf("Bravo, vous avez trouvé la bonne combinaison !\n");
  }
cleanup:

  h_close(socket);
  free(tampon_read);
  free(tampon_write);
}

/*****************************************************************************/
