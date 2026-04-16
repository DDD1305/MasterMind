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
void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{

  int num_socket = h_socket(AF_INET, SOCK_STREAM);
  struct sockaddr_in *adr;
  adr_socket(service, NULL, SOCK_STREAM, &adr);
  h_bind(num_socket, adr);
  h_listen(num_socket, 32);
  int num_socNew = h_accept(num_socket, adr);
  int reads = 1;
  int niveau_reseau;
  reads = h_reads(num_socNew, (char *)&niveau_reseau, sizeof(int));
  if (reads != sizeof(int)) {
    printf("n mauvais format");
    return;
  }

  n = ntohl(niveau_reseau);
  init(n);
  while (win && reads) {
    char buffer[n];
    reads = h_reads(num_socNew, buffer, n);
    /*jeu*/
    char tampon[16];
    int len_tampon = 16;
    int writes = h_writes(num_socNew, tampon, len_tampon);
  }
  h_close(num_socket);
  h_close(num_socNew);
}

/******************************************************************************/
