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
#include <set>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/wait.h>

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

void send_msg(char *msg) { h_writes(int num_soc, char *tampon, int nb_octets) }

char *receive_msg() {}

void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
  init_TCP_com(service);
  int reads = 1;
  int niveau_reseau;
  reads = h_reads(client.num_socketCom, (char *)&niveau_reseau, sizeof(int));
  // Traveau ici comment faire la diff et enfaite le cleinet sait trop de chose
  if (reads != sizeof(int)) {
    printf("n mauvais format\n");
    return;
  }
  n = ntohl(niveau_reseau);
  if (n <= 0 || n > 100) {
    printf("niveau invalide\n");
    return;
  }
  init(n);
  h_writes(num_socNew, "Entrer la proposition",
           strlen("Entrer la proposition"));
  int taille_buff = 4096;
  while (!win && reads) {
    char buffer[n];
    reads = h_reads(num_socNew, buffer, n);
    if (reads != n) {
      printf("msg pas de la bonne taille ou client deconnecte\n");
      break;
    }

    char *try = check(buffer, n);
    int writes = h_writes(num_socNew, try, n); // On envoie "TMF..."
    if (writes != n) {
      printf("probleme ecriture\n");
      free(try);
      break;
    }

    // Si la fonction check a passé win à 1, la partie est finie
    if (win) {
      free(try);
      break;
    }

    printf("\n");
    // Seulement si le jeu continue, on redemande une proposition
    h_writes(num_socNew, "Entrer la proposition",
             strlen("Entrer la proposition"));

    free(try); // Très important : empêche la fuite de mémoire à chaque tour
  }
  h_close(num_socket);
  h_close(num_socNew);
}

/******************************************************************************/
