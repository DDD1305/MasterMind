/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/

#include <curses.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fon.h" /* Primitives de la boite a outils */

#include "jeu.h"

#define SERVICE_DEFAUT "1111"

void serveur_appli(char *service); /* programme serveur */

/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

int main(int argc, char *argv[]) {

  char *service = SERVICE_DEFAUT; /* numero de service par defaut */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc) {
  case 1:
    printf("defaut service = %s\n", service);
    break;
  case 2:
    service = argv[1];
    break;

  default:
    printf("Usage:serveur service (nom ou port) \n");
    exit(1);
  }

  /* service est le service (ou numero de port) auquel sera affecte
  ce serveur*/

  serveur_appli(service);
}

/******************************************************************************/

typedef struct {
  int num_socketInit;
  int num_socketCom;
} socket_tcp;

socket_tcp client;

void init_TCP_com(char *service) {
  client.num_socketInit = h_socket(AF_INET, SOCK_STREAM);
  struct sockaddr_in *adr;
  adr_socket(service, NULL, SOCK_STREAM, &adr);
  h_bind(client.num_socketInit, adr);
  h_listen(client.num_socketInit, 32);
  client.num_socketCom = h_accept(client.num_socketInit, adr);
}

int send_msg(socket_tcp socket, char *msg) {
  int len = strlen(msg);
  int write = h_writes(socket.num_socketCom, (char *)&len,
                       sizeof(int)); // d'abord la taille du msg
  if (write != sizeof(int)) {
    return -1;
  }
  write = h_writes(socket.num_socketCom, msg, len);
  if (write != len) {
    return -1;
  }
  return write;
}

int receive_msg(socket_tcp socket, char *tampon, int taille) {
  int len;
  int read = h_reads(socket.num_socketCom, (char *)&len,
                     sizeof(int)); // On lit d'abord la taille
  if (read != sizeof(int)) {
    return -1;
  }
  if (len > taille) {
    printf("buffer tampon trop petit");
    return -1;
  }
  read = h_reads(socket.num_socketCom, tampon, len);
  if (read != len) {
    return -1;
  }
  return read; // retourne la taille du msg lu ou -1 si erreur.
}

int receive_level(socket_tcp socket) {
  int niveau_reseau;
  int reads =
      h_reads(socket.num_socketCom, (char *)&niveau_reseau, sizeof(int));
  if (reads != sizeof(int)) {
    printf("n mauvais format\n");
    return -1;
  }
  level = ntohl(niveau_reseau);
  if (level <= 0 || level > 100) {
    printf("niveau invalide\n");
    return -1;
  }
  return 0; // pas d'erreur
}

void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
  init_TCP_com(service);

  // On récupère le niveau
  int read = receive_level(client);
  if (read == -1) {
    printf("Erreur récupération level");
    goto cleanup;
  }

  // On initialise le jeu.
  init(level);
  char *answer = NULL;
  char *try = malloc(n + 1); // pour avoir la chaine de taille n et \0
  if (try == NULL) {
    printf("erreur alloction try");
    goto cleanup;
  }
  while (!win) {
    int send = send_msg(client, "Entrer la proposition:\n");
    if (send == -1) {
      printf("Erreur à l'envoie : Entrer une proposition\n");
      goto cleanup;
    }
    int read = receive_msg(client, try, n + 1); // fct bloquante
    if (read == -1) {
      goto cleanup;
    }
    answer = check(try, n);
    send = send_msg(client, answer);
    if (send == -1) {
      printf("Erreur à l'envoie la correction\n");
      goto cleanup;
    }
    free(answer);
  }

cleanup:
  if (try != NULL) {
    free(try);
  }
  if (answer != NULL) {
    free(answer);
  }
  h_close(client.num_socketCom);
  h_close(client.num_socketInit);
}

/******************************************************************************/
